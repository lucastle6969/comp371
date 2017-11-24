//example skeleton code
//modified from http://learnopengl.com/

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#define GLFW_INCLUDE_NONE // don't include deprecated gl headers on macOS
#include <GLFW/glfw3.h>	// include GLFW helper library

#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "glsetup.hpp"       // include gl context setup function
#include "shaderprogram.hpp" // include the shader program compiler
#include "src/entities/Entity.hpp"
#include "src/entities/World.hpp"
#include "src/entities/Player.hpp"
#include "constants.hpp"
#include "TreeDistributor.hpp"
#include "loadTexture.hpp"

World* world;

// Camera constants
const glm::vec3 up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
// pitch and yaw stored in degrees for clarity, but must be converted
// to radians to work well with glm
const float initial_pitch = -65.0f;
const float initial_yaw = -90.0f;
const float max_pitch = 89.0f;
const float min_pitch = -89.0f;
const float first_person_pitch = 20.0f;
const float max_follow_distance = 300.0f;
const float first_person_follow_distance = 0.01f;

// Camera variables
float pitch = initial_pitch;
float yaw = initial_yaw;

// Projection constants
float min_fovy = 45.0f; // degrees
// up to 180 is "ok" but it looks really distorted and we can easily see tiles loading.
// 120 is a pretty safe maximum that allows a good field of view but preserves the
// "endless world" illusion reasonably well.
float max_fovy = 120.0f;

// Projection variables, to be set by framebufferSizeCallback
int framebuffer_width = 0;
int framebuffer_height = 0;
// Perspective field of view y angle to be set in scrollCallback (stored here in degrees)
float fovy = 60.0f;

//Shadown width and Height for ViewPort
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

float getPlayerScaleCoefficient()
{
	glm::vec3 scale_vec = world->getPlayer()->getScale();
	return (scale_vec.x + scale_vec.y + scale_vec.z) / 3.0f;
}

glm::vec3 getViewDirection() {
	return glm::vec3(
			(float)(cos(glm::radians(yaw)) * cos(glm::radians(pitch))),
			(float)sin(glm::radians(pitch)),
			(float)(sin(glm::radians(yaw)) * cos(glm::radians(pitch)))
	);
}

glm::vec3 getFollowVector() {
	return glm::normalize(getViewDirection()) *
			max_follow_distance *
			// scale follow distance according to player size so player always
			// takes up same proportion of screen for a given viewing angle
			getPlayerScaleCoefficient() *
			// The lower the viewing angle, the shorter the follow distance -
			// to accommodate for less space near terrain. At a specified high pitch,
			// the third-person camera becomes first-person.
			std::max(
				first_person_follow_distance,
				(1 - (pitch - min_pitch) / (first_person_pitch - min_pitch))
			);
}

bool isKeyPressed(GLFWwindow* const& window, const int& key) {
	return glfwGetKey(window, key) == GLFW_PRESS;
}

// controls that should be polled at every frame and read
// continuously / in combination
void pollContinuousControls(GLFWwindow* window) {
	Player* player = world->getPlayer();
	// move forward
	if (isKeyPressed(window, GLFW_KEY_W) || isKeyPressed(window, GLFW_KEY_UP)) {
		player->moveForward(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	}
	// move back
	if (isKeyPressed(window, GLFW_KEY_S) || isKeyPressed(window, GLFW_KEY_DOWN)) {
		player->moveBack(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	}
	// move left
	if (isKeyPressed(window, GLFW_KEY_A) || isKeyPressed(window, GLFW_KEY_LEFT)) {
		player->moveLeft(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	}
	// move right
	if (isKeyPressed(window, GLFW_KEY_D) || isKeyPressed(window, GLFW_KEY_RIGHT)) {
		player->moveRight(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	}
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ignore key release actions for now
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			case GLFW_KEY_GRAVE_ACCENT:
				world->toggleAxes();
				break;
			case GLFW_KEY_BACKSPACE:
				// Reset camera
				pitch = initial_pitch;
				yaw = initial_yaw;
				break;
			case GLFW_KEY_ESCAPE:
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;
			default:
				break;
		}
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	// Thanks to https://learnopengl.com/#!Getting-started/Camera for helping
	// me think about camera movement!

	static bool was_mouse_captured = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

	static double last_xpos = xpos;
	static double last_ypos = ypos;

	static float sensitivity = 0.2f;

	bool is_mouse_captured = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
	if (!is_mouse_captured) {
		was_mouse_captured = false;
		// we don't want to handle camera movement if the mouse isn't captured
		return;
	}
	if (!was_mouse_captured) {
		// don't jerk the camera around if we're recapturing the mouse
		last_xpos = xpos;
		last_ypos = ypos;
	}
	was_mouse_captured = true;

	auto x_diff = sensitivity * (float)(xpos - last_xpos);
	auto y_diff = sensitivity * (float)(last_ypos - ypos);
	last_xpos = xpos;
	last_ypos = ypos;

	yaw += x_diff;
	pitch += y_diff;

	// set some vertical limits to avoid weird behavior
	if (pitch > max_pitch) {
		pitch = max_pitch;
	} else if (pitch < min_pitch) {
		pitch = min_pitch;
	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	fovy = glm::clamp(float(fovy + yoffset * 5.0f), min_fovy, max_fovy);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	// Update the viewport dimensions
	glViewport(0, 0, width, height);

	// update projection matrix variables to maintain aspect ratio
	framebuffer_width = width;
	framebuffer_height = height;
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	GLFWwindow* window = nullptr;
	setupGlContext(WIDTH, HEIGHT, APP_NAME, &window);

	// Set the required callback functions
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Set up viewport and projection matrix, which will be updated whenever the framebuffer resizes.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	framebufferSizeCallback(window, width, height);
    
    
    //Create Depth Buffer for Shadown rendering
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    
    //2D texture that we'll use as the framebuffer's depth buffer
    
    unsigned int depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapFBO, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    
    // Load the texture
    GLuint Texture = loadDDS("../textures/uvmap.DDS");
    
    
    

	bool shader_program_ok;

	GLuint shader_program = prepareShaderProgram(
		"../shaders/vertex.glsl",
		"../shaders/fragment.glsl",
		&shader_program_ok
	);
	if (!shader_program_ok) {
		return -1;
	}

	world = new World(shader_program);
    //create light

    Light light(glm::vec3(0, -1, 0), glm::vec3(.5, .5, .5));

    
    // Get a handle for our sampler2D
    GLuint TextureID  = glGetUniformLocation(shader_program, "myTextureSampler");
    //Get a handle for sampler2DShadow
    GLuint ShadowMapID = glGetUniformLocation(shader_program, "shadowMap");
    


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
        
        // 1. render depth of scene to texture (from light's perspective)
        
        // Render to our framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT); // Render on the whole framebuffer, complete from the lower left corner to the upper right
        
        
        // We don't use bias in the shader, but instead we draw back faces,
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
        
        //Only depth, no draw
        glClear(GL_DEPTH_BUFFER_BIT );
        
        
        
        // Compute the MVP matrix from the light's point of view
        glm::mat4 lightProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
        glm::mat4 lightViewMatrix = glm::lookAt(light.light_direction, glm::vec3(0,0,0), glm::vec3(0,1,0));
        
        
        
        
        
        
        //Setting isShadowMapping to 1 , and sending our lightMVP to the shaders
        world->drawShadowMap(lightViewMatrix, lightProjectionMatrix);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        
        
        
        
        
        
        // 2. Render scene as normal using the generated shadow map
        
        glViewport(0,0,width, height);
        
		static glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
		static glm::vec3 y_axis(0.0f, 1.0f, 0.0f);

		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		pollContinuousControls(window);

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
        
        
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glUniform1i(ShadowMapID, 1);
        
        
        
		glm::vec3 player_position = world->getPlayer()->getPosition();
		glm::mat4 view_matrix = glm::lookAt(player_position - getFollowVector(), player_position, up);

		float player_scale = getPlayerScaleCoefficient();
		glm::mat4 projection_matrix = glm::perspective(
			glm::radians(fovy),
			(GLfloat)framebuffer_width / (GLfloat)framebuffer_height,
			15.0f * player_scale,
			1500.0f * player_scale
		);

		world->draw(view_matrix, projection_matrix, light);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	delete world;
    glDeleteFramebuffers(1, &depthMapFBO);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

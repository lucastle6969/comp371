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
#include "src/entities/Skybox.hpp"

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
	bool up_press = isKeyPressed(window, GLFW_KEY_W) || isKeyPressed(window, GLFW_KEY_UP);
	bool down_press = isKeyPressed(window, GLFW_KEY_S) || isKeyPressed(window, GLFW_KEY_DOWN);
	bool left_press = isKeyPressed(window, GLFW_KEY_A) || isKeyPressed(window, GLFW_KEY_LEFT);
	bool right_press = isKeyPressed(window, GLFW_KEY_D) || isKeyPressed(window, GLFW_KEY_RIGHT);

	// ignore action canceling button presses
	if (up_press && down_press) {
		up_press = down_press = false;
	}
	if (left_press && right_press) {
		left_press = right_press = false;
	}

	// first check compound then single movement button actions
	if (up_press && left_press) {
		player->moveForwardLeft(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	} else if (up_press && right_press) {
		player->moveForwardRight(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	} else if (down_press && left_press) {
		player->moveBackLeft(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	} else if (down_press && right_press) {
		player->moveBackRight(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	} else if (up_press) {
		player->moveForward(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	} else if (down_press) {
		player->moveBack(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	} else if (left_press) {
		player->moveLeft(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
		world->checkPosition();
	} else if (right_press) {
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
	//create skybox
	Skybox skybox(shader_program);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		static glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
		static glm::vec3 y_axis(0.0f, 1.0f, 0.0f);

		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		pollContinuousControls(window);

		glm::vec3 follow_vector = getFollowVector();

		world->getPlayer()->setOpacity(
			(glm::length(follow_vector) - getPlayerScaleCoefficient() * 30.0f) /
				(getPlayerScaleCoefficient() * 50.0f)
		);

		// rotate the sun
		light.daytime = glm::rotateZ(light.daytime, 0.0005f);
        //move the fog
		glm::vec3 player_position = world->getPlayer()->getPosition();
		light.light_position = player_position;
		skybox.setPosition(player_position);

		light.setDaytime();

		float daytime = -light.light_direction.y;
		// Render
		// Clear the colorbuffer
		glClearColor(light.fog_color.r, light.fog_color.g, light.fog_color.b , 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view_matrix = glm::lookAt(player_position - follow_vector, player_position, up);


		float player_scale = getPlayerScaleCoefficient();
		glm::mat4 projection_matrix = glm::perspective(
			glm::radians(fovy),
			(GLfloat)framebuffer_width / (GLfloat)framebuffer_height,
			15.0f * player_scale,
			1500.0f * player_scale
		);

		glm::mat4 sky_projection_matrix = glm::perspective(
				glm::radians(fovy),
				(GLfloat)framebuffer_width / (GLfloat)framebuffer_height,
				0.1f * player_scale,
				1000000.0f * player_scale
		);
		skybox.draw(view_matrix, sky_projection_matrix, light);

		world->draw(view_matrix, projection_matrix, light);
				// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	delete world;

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

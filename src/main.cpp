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

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "glsetup.hpp"       // include gl context setup function
#include "shaderprogram.hpp" // include the shader program compiler
#include "entities/entity.hpp"
#include "entities/drawableentity.hpp"
#include "entities/heightmapterrain.hpp"
#include "entities/worldorigin.hpp"
#include "entities/player.hpp"

const char* APP_NAME = "Procedural World";

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

const std::string DEFAULT_IMAGE_FILE = "../depth.png";
const int DEFAULT_SKIP_SIZE = 20;
const float DEFAULT_INTERPOLATION_SIZE = 0.1;

// bounds for placing grid, axes and objects
const int WORLD_X_MIN = -10;
const int WORLD_X_MAX = 10;
const int WORLD_Y_MIN = -10;
const int WORLD_Y_MAX = 10;
// this one is only used for the z-axis
const int WORLD_Z_MAX = 10;

const float PLAYER_MOVEMENT_SPEED = 0.1;

glm::mat4 projection_matrix;

std::vector<DrawableEntity*> entities;

WorldOrigin* origin;
HeightMapTerrain* height_map_terrain;
Player* player;

// Player constants
const glm::vec3 initial_player_position(0.0f, 2.3f, 0.0f);

// Camera constants
const glm::vec3 up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
// pitch and yaw stored in degrees for clarity, but must be converted
// to radians to work well with glm
const float initial_pitch = -65.0f;
const float initial_yaw = -90.0f;
const float max_pitch = 89.0f;
const float min_pitch = -89.0f;
const float max_follow_distance = 10.0f;

// Camera variables
float pitch = initial_pitch;
float yaw = initial_yaw;

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
			// The lower the viewing angle, the shorter the follow distance -
			// to accommodate for less space near terrain. At our lowest viewing
			// angle, the third-person camera becomes first-person.
			(1 - (pitch - min_pitch) / (max_pitch - min_pitch));
}

bool isKeyPressed(GLFWwindow* const& window, const int& key) {
	return glfwGetKey(window, key) == GLFW_PRESS;
}

// controls that should be polled at every frame and read
// continuously / in combination
void pollContinuousControls(GLFWwindow* window) {
	// move forward
	if (isKeyPressed(window, GLFW_KEY_W) || isKeyPressed(window, GLFW_KEY_UP)) {
		player->moveForward(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
	}
	// move back
	if (isKeyPressed(window, GLFW_KEY_S) || isKeyPressed(window, GLFW_KEY_DOWN)) {
		player->moveBack(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
	}
	// move left
	if (isKeyPressed(window, GLFW_KEY_A) || isKeyPressed(window, GLFW_KEY_LEFT)) {
		player->moveLeft(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
	}
	// move right
	if (isKeyPressed(window, GLFW_KEY_D) || isKeyPressed(window, GLFW_KEY_RIGHT)) {
		player->moveRight(getViewDirection(), up, PLAYER_MOVEMENT_SPEED);
	}
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ignore key release actions for now
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			case GLFW_KEY_1:
				height_map_terrain->selectStep(1);
				break;
			case GLFW_KEY_2:
				height_map_terrain->selectStep(2);
				break;
			case GLFW_KEY_3:
				height_map_terrain->selectStep(3);
				break;
			case GLFW_KEY_4:
				height_map_terrain->selectStep(4);
				break;
			case GLFW_KEY_GRAVE_ACCENT:
				origin->toggle_hide();
				break;
			case GLFW_KEY_BACKSPACE:
				// Reset terrain
				height_map_terrain->selectStep(1);
				// Reset camera
				pitch = initial_pitch;
				yaw = initial_yaw;
				break;
			case GLFW_KEY_P:
			case GLFW_KEY_L:
			case GLFW_KEY_T: {
				GLenum draw_mode = GL_POINTS;
				if (key == GLFW_KEY_L) draw_mode = GL_LINES;
				if (key == GLFW_KEY_T) draw_mode = GL_TRIANGLES;
				// set each object's draw mode to match the key that was pressed:
				// P = points, L = lines, T = triangles
				for (DrawableEntity *entity : entities) {
					if (entity == origin) continue;
					entity->setDrawMode(draw_mode);
				}
				break;
			}
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

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	// Update the viewport dimensions
	glViewport(0, 0, width, height);

	// Update projection matrix to maintain aspect ratio
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void promptForUserInputs()
{
	std::string input;

	int skip_size = 0;
	do {
		std::cout << "Choose a skip size greater than 0 [" << DEFAULT_SKIP_SIZE << "]: ";
		// skip for now: // std::getline(std::cin, input);
		if (input.empty()) {
			skip_size = DEFAULT_SKIP_SIZE;
			break;
		} else {
			try {
				skip_size = std::stoi(input);
			} catch(std::invalid_argument& e) {
				// try again
			}
		}
	} while (skip_size <= 0);

    float interpolation_size = 0;
    // Enforce a reasonable limit
    float interpolation_size_limit = 0.5f;
    do {
        std::cout << "Choose a step size greater than 0 and less than or equal to ";
        std::cout << interpolation_size_limit << " [" << DEFAULT_INTERPOLATION_SIZE << "]: ";
	    // skip for now: // std::getline(std::cin, input);
        if (input.empty()) {
            interpolation_size = DEFAULT_INTERPOLATION_SIZE;
            break;
        } else {
            try {
                interpolation_size = std::stof(input);
            } catch(std::invalid_argument& e) {
                // try again
            }
        }
    } while (interpolation_size <= 0 || interpolation_size > interpolation_size_limit);

	height_map_terrain->setUserInputs(skip_size, interpolation_size);
	height_map_terrain->selectStep(2);
	std::cout << "Vertices loaded!\n";
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
    std::cout << "Choose an image to load [" << DEFAULT_IMAGE_FILE << "]: ";
    std::string image_file;
    // skip for now: // std::getline(std::cin, image_file);
    if (image_file.empty()) {
        image_file = DEFAULT_IMAGE_FILE;
    }

	GLFWwindow* window = nullptr;
	setupGlContext(WIDTH, HEIGHT, APP_NAME, &window);

	// Set the required callback functions
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Set up viewport and projection matrix, which will be updated whenever the framebuffer resizes.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	framebufferSizeCallback(window, width, height);

	bool shader_program_ok;
	GLuint shader_program = prepareShaderProgram("../shaders/vertex.glsl", "../shaders/fragment.glsl",
	                                             &shader_program_ok);
	if (!shader_program_ok) {
		return -1;
	}

	origin = new WorldOrigin(shader_program, WORLD_X_MAX, WORLD_Y_MAX, WORLD_Z_MAX);
	origin->hide();
	// copy pointer to entity list
	entities.push_back(&*origin);

	height_map_terrain = new HeightMapTerrain(shader_program, image_file, origin);
	height_map_terrain->scale(0.024f);
	// copy pointer to entity list
	entities.push_back(&*height_map_terrain);

	player = new Player(shader_program, origin);
	player->scale(0.04f);
	player->setPosition(initial_player_position);
	// copy pointer to entity list
	entities.push_back(&*player);

	// doesn't actually prompt anymore
	promptForUserInputs();

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		static glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
		static glm::vec3 y_axis(0.0f, 1.0f, 0.0f);

		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		pollContinuousControls(window);

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 player_position = player->getPosition();
		glm::mat4 view_matrix = glm::lookAt(player_position - getFollowVector(), player_position, up);

		for (DrawableEntity* entity : entities) {
			entity->draw(view_matrix, projection_matrix);
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// De-allocate the memory for all our entities
	for (DrawableEntity* entity : entities) {
		delete entity;
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

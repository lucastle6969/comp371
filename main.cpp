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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "glsetup.hpp"       // include gl context setup function
#include "shaderprogram.hpp" // include the shader program compiler
#include "entity.hpp"
#include "heightmapterrain.hpp"
#include "worldorigin.hpp"
#include "utils.hpp"

const char* APP_NAME = "Height Mapper";

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

glm::mat4 projection_matrix;

std::vector<Entity*> entities;

WorldOrigin* origin;
HeightMapTerrain* height_map_terrain;

// Camera constants
const glm::vec3 up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
// pitch and yaw stored in degrees for clarity, but must be converted
// to radians to work well with glm
const float initial_pitch = -65.0f;
const float initial_yaw = -90.0f;
const glm::vec3 initial_eye(0.0f, 20.0f, 10.0f);

// Camera variables
float pitch = initial_pitch;
float yaw = initial_yaw;
glm::vec3 eye = initial_eye;

bool awaiting_user_input = false;

glm::vec3 getViewDirection() {
	return glm::vec3(
			(float)(cos(glm::radians(yaw)) * cos(glm::radians(pitch))),
			(float)sin(glm::radians(pitch)),
			(float)(sin(glm::radians(yaw)) * cos(glm::radians(pitch)))
	);
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
	static glm::vec3 z_axis(0.0f, 0.0f, 1.0f);

	glm::vec3 view_direction = getViewDirection();
	glm::vec3 camera_center = eye + view_direction;
	glm::vec3 left_direction = glm::cross(up, view_direction);
	glm::vec3 forward_direction = glm::cross(left_direction, up);

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
			case GLFW_KEY_W:
			case GLFW_KEY_UP:
				// move forward
				eye += 0.3f * glm::normalize(forward_direction);
				break;
			case GLFW_KEY_S:
			case GLFW_KEY_DOWN:
				// move backward
				eye -= 0.3f * glm::normalize(forward_direction);
				break;
			case GLFW_KEY_A:
			case GLFW_KEY_LEFT:
				// move left
				eye += 0.3f * glm::normalize(left_direction);
				break;
			case GLFW_KEY_D:
			case GLFW_KEY_RIGHT:
				// move right
				eye -= 0.3f * glm::normalize(left_direction);
				break;
			case GLFW_KEY_SPACE:
				// move up
				eye += 0.3f * up;
				break;
			case GLFW_KEY_TAB:
				// move down
				eye -= 0.3f * up;
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
				eye = initial_eye;
				// Request user input
				awaiting_user_input = true;
				break;
			case GLFW_KEY_P:
			case GLFW_KEY_L:
			case GLFW_KEY_T: {
				GLenum draw_mode = GL_POINTS;
				if (key == GLFW_KEY_L) draw_mode = GL_LINES;
				if (key == GLFW_KEY_T) draw_mode = GL_TRIANGLES;
				// set each object's draw mode to match the key that was pressed:
				// P = points, L = lines, T = triangles
				for (Entity *entity : entities) {
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
	if (pitch > 89.0f) {
		pitch = 89.0f;
	} else if (pitch < -89.0f) {
		pitch = -89.0f;
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
	awaiting_user_input = false;
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
	GLuint shader_program = prepareShaderProgram("../vertex.glsl", "../fragment.glsl",
	                                             &shader_program_ok);
	if (!shader_program_ok) {
		return -1;
	}
	glUseProgram(shader_program);

	origin = new WorldOrigin(shader_program, WORLD_X_MAX, WORLD_Y_MAX, WORLD_Z_MAX);
	origin->hide();
	// copy pointer to entity list
	entities.push_back(&*origin);

	height_map_terrain = new HeightMapTerrain(shader_program, image_file, origin);
	height_map_terrain->scale(0.024f);
	// copy pointer to entity list
	entities.push_back(&*height_map_terrain);

	auto mvp_matrix_loc = (GLuint)glGetUniformLocation(shader_program, "mvp_matrix");
	auto color_type_loc = (GLuint)glGetUniformLocation(shader_program, "color_type");

	// Game loop
	bool rendered_at_least_once = false;
	while (!glfwWindowShouldClose(window))
	{
		static glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
		static glm::vec3 y_axis(0.0f, 1.0f, 0.0f);

		if (awaiting_user_input) {
			// since it happens _before_ glfwPollEvents() is called, this code
			// won't be reached until at least one render since the program was reset
			promptForUserInputs();
		}

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view_matrix = glm::lookAt(eye, eye + getViewDirection(), up);

		for (Entity* entity : entities) {
			// Skip to the next entity if the current entity is hidden
			if (entity->isHidden()) continue;

			// use the entity's model matrix to form a new Model View Projection matrix
			glm::mat4 mvp_matrix = projection_matrix * view_matrix * entity->getModelMatrix();
			// send the mvp_matrix variable content to the shader
			glUniformMatrix4fv(mvp_matrix_loc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
			// send the color_type variable to the shader (could be null)
			glUniform1i(color_type_loc, entity->getColorType());

			// Draw
			glBindVertexArray(entity->getVAO());
			GLenum draw_mode = entity->getDrawMode();
			if (draw_mode == GL_POINTS) {
				// it's inefficient and useless to use glDrawElements for a point cloud
				glDrawArrays(draw_mode, 0, (GLuint)entity->getVertices().size());
			} else {
				int elementBufferArraySize;
				glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &elementBufferArraySize);
				glDrawElements(
						draw_mode,
						elementBufferArraySize / sizeof(GLuint),
						GL_UNSIGNED_INT,
						nullptr
				);
			}
			glBindVertexArray(0);
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);

		if (!rendered_at_least_once) {
			rendered_at_least_once = true;
			// now that we're rendering something we'll request a skip size prompt
			awaiting_user_input = true;
		}
	}

	// De-allocate the memory for all our entities
	for (Entity* entity : entities) {
		delete entity;
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

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

#include "glsetup.hpp"       // include gl context setup function
#include "shaderprogram.hpp" // include the shader program compiler
#include "entity.hpp"
#include "heightmapterrain.hpp"
#include "worldorigin.hpp"
#include "utils.hpp"

const char* APP_NAME = "Height Mapper";

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

const int DEFAULT_SKIP_SIZE = 20;

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

// Camera
const glm::vec3 up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
glm::vec3 eye(0.0f, 20.0f, 3.0f);
glm::vec3 view_direction(0.0f, -1.0f, -0.1f);

bool awaiting_skip_size_prompt = false;

glm::vec3 getCameraCenter()
{
	// the center position should move at a fixed displacement ahead of the viewing position
	return eye + view_direction;
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	static glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
	static glm::vec3 z_axis(0.0f, 0.0f, 1.0f);

	glm::vec3 camera_center = getCameraCenter();
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
				// move up or down
				eye += (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? -0.3f : 0.3f) * up;
				break;
			case GLFW_KEY_BACKSPACE:
				// Reset program
				height_map_terrain->selectStep(1);
				// TODO: reset camera, whatever that will mean
				awaiting_skip_size_prompt = true;
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
			default:
				break;
		}
	}
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	// TODO: rotate view_direction
	/*
	static double last_xpos = xpos;
	static double last_ypos = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		// if the left mouse button is held down, dolly the camera in or out
		// according to the mouse movement
		// (drag up = dolly in, drag down = dolly out)
		eye.z += 0.1f * (ypos - last_ypos);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		// if the right mouse button is held down, pan the camera left and
		// right according to the mouse movement
		double diff = 0.01 * (xpos - last_xpos);
		eye.x += diff;
		center.x += diff;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		// if the middle mouse button is held down, tilt the camera up and
		// down according to the mouse movement
		tilt_angle += 0.001f * (ypos - last_ypos);
	}

	last_xpos = xpos;
	last_ypos = ypos;
	 */
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	// Update the viewport dimensions
	glViewport(0, 0, width, height);

	// Update projection matrix to maintain aspect ratio
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void promptForSkipSize()
{
	std::string input;
	int skip_size = 0;
	do {
		std::cout << "Choose a skip size greater than 0 [" << DEFAULT_SKIP_SIZE << "]: ";
		std::getline(std::cin, input);
		if (input.empty()) {
			skip_size = DEFAULT_SKIP_SIZE;
			break;
		} else {
			try {
				skip_size = std::stoi(input);
			} catch(std::invalid_argument e) {
				// try again
			}
		}
	} while (skip_size <= 0);

	height_map_terrain->setSkipSize(skip_size);
	height_map_terrain->selectStep(2);
	awaiting_skip_size_prompt = false;
	std::cout << "Vertices loaded!\n";
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	GLFWwindow* window = nullptr;
	setupGlContext(WIDTH, HEIGHT, APP_NAME, &window);

	// Set the required callback functions
	glfwSetKeyCallback(window, keyCallback);
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
	// copy pointer to entity list
	entities.push_back(&*origin);

	height_map_terrain = new HeightMapTerrain(shader_program, "../depth.bmp", origin);
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

		if (awaiting_skip_size_prompt) {
			// since it happens _before_ glfwPollEvents() is called, this code
			// won't be reached until at least one render since the program was reset
			promptForSkipSize();
		}

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view_matrix = glm::lookAt(eye, getCameraCenter(), up);

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
			awaiting_skip_size_prompt = true;
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

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
#include "worldorigin.hpp"
#include "grid.hpp"
#include "pacman.hpp"
#include "dot.hpp"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

const GLuint NUM_DOTS = 25;

const int WORLD_X_MIN = -10;
const int WORLD_X_MAX = 10;
const int WORLD_Y_MIN = -10;
const int WORLD_Y_MAX = 10;
const int WORLD_Z_MIN = -10;
const int WORLD_Z_MAX = 10;

glm::mat4 projection_matrix;

Pacman* pacman;

// Constant vectors
const glm::vec3 center(0.0f, 0.0f, 0.0f);
const glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 eye(0.0f, 0.0f, 20.0f);

time_t last_movement_tick = 0;
const clock_t MOVE_WAITING_TIME = 2500;
bool canMoveAgain()
{
	clock_t t = clock();
	if (t - last_movement_tick < MOVE_WAITING_TIME) {
		return false;
	}
	last_movement_tick = t;
	return true;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	switch (key) {
		case 265: // up
			eye.y += 1;
			break;
		case 264: // down
			eye.y -=1;
			break;
		case 263: // left
			eye.x -= 1;
			break;
		case 262: // right
			eye.x += 1;
			break;
		case 87: // w
			if (canMoveAgain() && pacman->getPosition().y < WORLD_Y_MAX) {
				pacman->moveUp();
			}
			break;
		case 65: // a
			if (canMoveAgain() && pacman->getPosition().x > WORLD_X_MIN) {
				pacman->moveLeft();
			}
			break;
		case 83: // s
			if (canMoveAgain() && pacman->getPosition().y > WORLD_Y_MIN) {
				pacman->moveDown();
			}
			break;
		case 68: // d
			if (canMoveAgain() && pacman->getPosition().x < WORLD_X_MAX) {
				pacman->moveRight();
			}
		default:
			break;
	}
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	GLFWwindow* window = nullptr;
	setupGlContext(WIDTH, HEIGHT, "Pacman", &window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 2.0f, 100.0f);

	bool shader_program_ok;
	GLuint shader_program = prepareShaderProgram("../vertex.glsl", "../fragment.glsl",
	                                             &shader_program_ok);
	if (!shader_program_ok) {
		return -1;
	}
	glUseProgram(shader_program);

	std::vector<Entity*> entities;

	auto origin = new WorldOrigin(shader_program, WORLD_X_MAX, WORLD_Y_MAX, WORLD_Z_MAX);
	// copy pointer to entity list
	entities.push_back(&*origin);

	auto grid = new Grid(shader_program, WORLD_X_MIN, WORLD_X_MAX, WORLD_Y_MIN, WORLD_Y_MAX, origin);
	// copy pointer to entity list
	entities.push_back(&*grid);

	pacman = new Pacman(shader_program, grid);
	pacman->scale(0.04f);
	// copy pointer to entity list
	entities.push_back(&*pacman);

	// add several dot pickups to the grid
	for (int i = 0; i < NUM_DOTS; i++) {
		// create a new dot
		auto dot = new Dot(shader_program, grid);
		dot->scale(0.2f);
		// place the dot randomly on the grid
		dot->setPosition(rand() % 21 - 10, rand() % 21 - 10);
		// copy the dot's pointer and include it in our entity list
		entities.push_back(&*dot);
	}

	auto mvp_matrix_loc = (GLuint)glGetUniformLocation(shader_program, "mvp_matrix");
	auto color_type_loc = (GLuint)glGetUniformLocation(shader_program, "color_type");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(eye, center, up);

		for (Entity* entity : entities) {
			// use the entity's model matrix to form a new Model View Projection matrix
			glm::mat4 mvp_matrix = projection_matrix * view_matrix * entity->getModelMatrix();
			// send the mvp_matrix variable content to the shader
			glUniformMatrix4fv(mvp_matrix_loc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
			// send the color_type variable to the shader (could be null)
			glUniform1i(color_type_loc, *entity->getColorType());

			GLuint* vao = entity->getVAO();
			std::vector<glm::vec3>* vertices = entity->getVertices();
			const GLenum* draw_mode = entity->getDrawMode();
			if (vao != nullptr && vertices != nullptr && draw_mode != nullptr) {
				// draw, if and ONLY if we have non-null VAO and vertices
				glBindVertexArray(*vao);
				glDrawArrays(*draw_mode, 0, (GLuint)(*vertices).size());
				glBindVertexArray(0);
			}
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Deallocate the memory for all our entities
	for (Entity* entity : entities) {
		delete entity;
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

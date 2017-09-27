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

WorldOrigin* origin;
Grid* grid;
Pacman* pacman;

std::vector<Dot*> dots;

// Constant vectors
const glm::vec3 center(0.0f, 0.0f, 0.0f);
const glm::vec3 up(0.0f, 1.0f, 0.0f);
const glm::vec3 eye(0.0f, 0.0f, 20.0f);

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
	static glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
	static glm::vec3 y_axis(0.0f, 1.0f, 0.0f);

	bool should_check_collisions = false;

	if (!action) {
		// for now we only handle keydown actions
		return;
	}
	switch (key) {
		case 265: // up
			origin->rotate(-0.1f, x_axis);
			break;
		case 264: // down
			origin->rotate(0.1f, x_axis);
			break;
		case 263: // left
			origin->rotate(-0.1f, y_axis);
			break;
		case 262: // right
			origin->rotate(0.1f, y_axis);
			break;
		case 87: // w
			if (canMoveAgain() && pacman->getPosition().y < WORLD_Y_MAX) {
				pacman->moveUp();
				should_check_collisions = true;
			}
			break;
		case 65: // a
			if (canMoveAgain() && pacman->getPosition().x > WORLD_X_MIN) {
				pacman->moveLeft();
				should_check_collisions = true;
			}
			break;
		case 83: // s
			if (canMoveAgain() && pacman->getPosition().y > WORLD_Y_MIN) {
				pacman->moveDown();
				should_check_collisions = true;
			}
			break;
		case 68: // d
			if (canMoveAgain() && pacman->getPosition().x < WORLD_X_MAX) {
				pacman->moveRight();
				should_check_collisions = true;
			}
			break;
		default:
			break;
	}

	if (should_check_collisions) {
		glm::vec3 pos = pacman->getPosition();
		glm::vec3 dot_pos;
		// Check if any of the dots overlap with pacman, and if so, remove them
		for (auto i = (int)dots.size(); i--; ) {
			dot_pos = dots[i]->getPosition();
			if (pos.x == dot_pos.x && pos.y == dot_pos.y) {
				// hide the dot from rendering
				dots[i]->hide();
				// remove the dot from our dots list (stays in entity list for de-allocation later)
				dots.erase(dots.begin() + i);
			}
		}
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

	origin = new WorldOrigin(shader_program, WORLD_X_MAX, WORLD_Y_MAX, WORLD_Z_MAX);
	// copy pointer to entity list
	entities.push_back(&*origin);

	grid = new Grid(shader_program, WORLD_X_MIN, WORLD_X_MAX, WORLD_Y_MIN, WORLD_Y_MAX, origin);
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
		int x_pos, y_pos;
		do {
			x_pos = rand() % 21 - 10;
			y_pos = rand() % 21 - 10;
		} while (x_pos == 0 && y_pos == 0);
		dot->setPosition(x_pos, y_pos);
		// copy the dot's pointer and include it in our entity list and dot list
		entities.push_back(&*dot);
		dots.push_back(dot);
	}

	auto mvp_matrix_loc = (GLuint)glGetUniformLocation(shader_program, "mvp_matrix");
	auto color_type_loc = (GLuint)glGetUniformLocation(shader_program, "color_type");

	glm::mat4 view_matrix;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view_matrix = glm::lookAt(eye, center, up);

		for (Entity* entity : entities) {
			// Check we're actually prepared to draw. If not, skip to next entity.
			if (entity->isHidden()) continue;
			GLuint* vao = entity->getVAO();
			if (vao == nullptr) continue;
			std::vector<glm::vec3>* vertices = entity->getVertices();
			if (vertices == nullptr) continue;
			const GLenum* draw_mode = entity->getDrawMode();
			if (draw_mode == nullptr) continue;

			// use the entity's model matrix to form a new Model View Projection matrix
			glm::mat4 mvp_matrix = projection_matrix * view_matrix * entity->getModelMatrix();
			// send the mvp_matrix variable content to the shader
			glUniformMatrix4fv(mvp_matrix_loc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
			// send the color_type variable to the shader (could be null)
			glUniform1i(color_type_loc, *entity->getColorType());

			// Draw
			glBindVertexArray(*vao);
			glDrawArrays(*draw_mode, 0, (GLuint)(*vertices).size());
			glBindVertexArray(0);
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// De-allocate the memory for all our entities
	for (Entity* entity : entities) {
		delete entity;
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

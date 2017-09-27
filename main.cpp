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

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

glm::mat4 projection_matrix;

// Constant vectors
const glm::vec3 center(0.0f, 0.0f, 0.0f);
const glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 eye(0.0f, 0.0f, 20.0f);

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == 264) {
		eye.y -= 1;
	}
	if (key == 265) {
		eye.y += 1;
	}
	if (key == 263) {
		eye.x -= 1;
	}
	if (key == 262) {
		eye.x += 1;
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

	WorldOrigin origin(shader_program);
	entities.push_back(&origin);

	Grid grid(shader_program, &origin);
	entities.push_back(&grid);

	Pacman pacman(shader_program, &grid);
	pacman.scale(0.04f);
	entities.push_back(&pacman);

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

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

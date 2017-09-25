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
#include "objloader.hpp"     // include the object loader
#include "constants.hpp"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

glm::vec3 camera_position;
glm::vec3 triangle_scale;
glm::vec3 pacman_scale;
glm::vec3 grid_scale;
glm::mat4 projection_matrix;

// Constant vectors
const glm::vec3 center(0.0f, 0.0f, 0.0f);
const glm::vec3 up(0.0f, 1.0f, 0.0f);
const glm::vec3 eye(0.0f, 0.0f, 50.0f);

void getGridVertices(const int& x_min, const int& x_max,
                     const int& y_min, const int& y_max,
                     std::vector<glm::vec3>* grid)
{
	// vertical lines
	for (int x = x_min; x <= x_max; x++) {
		grid->emplace_back(x, y_min, 0.0f);
		grid->emplace_back(x, x == 0 ? 0.0f : y_max, 0.0f); // leave room for y-axis
	}
	// horizontal lines
	for (int y = y_min; y <= y_max; y++) {
		grid->emplace_back(x_min, y, 0.0f);
		grid->emplace_back(y == 0 ? 0.0f : x_max, y, 0.0f); // leave room for x-axis
	}
}

void getAxisVertices(const int& x_max, const int& y_max, const int& z_max, std::vector<glm::vec3>* axes)
{
	// x-axis
	axes->emplace_back(0.0f, 0.0f, 0.0f);
	axes->emplace_back(x_max, 0.0f, 0.0f);
	// y-axis
	axes->emplace_back(0.0f, 0.0f, 0.0f);
	axes->emplace_back(0.0f, y_max, 0.0f);
	// z-axis
	axes->emplace_back(0.0f, 0.0f, 0.0f);
	axes->emplace_back(0.0f, 0.0f, z_max);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;	
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

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	loadOBJ("../pacman.obj", &vertices, &normals, &UVs); //read the vertices from the cube.obj file

	GLuint VAO;
	GLuint vertices_buffer;

	glGenVertexArrays(1, &VAO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glGenBuffers(1, &vertices_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);

	auto v_position = (GLuint)glGetAttribLocation(shader_program, "v_position");
	glEnableVertexAttribArray(v_position);
	glVertexAttribPointer(v_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

	// unbind buffer and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// generate grid vertices
	std::vector<glm::vec3> grid_vertices;
	getGridVertices(-10, 10, -10, 10, &grid_vertices);

	// create vao for grid
	GLuint VAO2, vertices_buffer_2;
	glGenVertexArrays(1, &VAO2);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO2);

	glGenBuffers(1, &vertices_buffer_2);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer_2);
	glBufferData(GL_ARRAY_BUFFER, grid_vertices.size() * sizeof(glm::vec3), &grid_vertices.front(), GL_STATIC_DRAW);

	auto v_position2 = (GLuint)glGetAttribLocation(shader_program, "v_position");
	glEnableVertexAttribArray(v_position2);
	glVertexAttribPointer(v_position2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

	// unbind buffer and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// generate axis vertices
	std::vector<glm::vec3> axis_vertices;
	getAxisVertices(10, 10, 10, &axis_vertices);

	// create vao for grid
	GLuint VAO3, vertices_buffer_3;
	glGenVertexArrays(1, &VAO3);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO3);

	glGenBuffers(1, &vertices_buffer_3);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer_3);
	glBufferData(GL_ARRAY_BUFFER, axis_vertices.size() * sizeof(glm::vec3), &axis_vertices.front(), GL_STATIC_DRAW);

	auto v_position3 = (GLuint)glGetAttribLocation(shader_program, "v_position");
	glEnableVertexAttribArray(v_position3);
	glVertexAttribPointer(v_position3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

	// unbind buffer and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	triangle_scale = glm::vec3(1.0f);
	pacman_scale = glm::vec3(0.1f);
	grid_scale = glm::vec3(25.0f);

	auto mvp_matrix_loc = (GLuint)glGetUniformLocation(shader_program, "mvp_matrix");

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

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, pacman_scale);

		glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

		glUniformMatrix4fv(mvp_matrix_loc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

		auto color_type_loc = (GLuint)glGetUniformLocation(shader_program, "color_type");
		glUniform1i(color_type_loc, COLOR_YELLOW);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, (GLuint)vertices.size());
		glBindVertexArray(0);

		model_matrix = glm::scale(model_matrix, grid_scale);

		mvp_matrix = projection_matrix * view_matrix * model_matrix;

		glUniformMatrix4fv(mvp_matrix_loc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

		auto color_type_loc2 = (GLuint)glGetUniformLocation(shader_program, "color_type");
		glUniform1i(color_type_loc2, COLOR_WHITE);

		glBindVertexArray(VAO2);
		glDrawArrays(GL_LINES, 0, (GLuint)grid_vertices.size());
		glBindVertexArray(0);

		auto color_type_loc3 = (GLuint)glGetUniformLocation(shader_program, "color_type");
		glUniform1i(color_type_loc3, COLOR_COORDINATE_AXES);

		glBindVertexArray(VAO3);
		glDrawArrays(GL_LINES, 0, (GLuint)axis_vertices.size());
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

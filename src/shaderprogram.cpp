#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include "constants.hpp"
#include "ShaderUniformLocationCache.hpp"

// Build and compile our shader program
GLuint prepareShaderProgram(const std::string& vertex_shader_path,
                            const std::string& fragment_shader_path,
                            bool* const& ok)
{
	*ok = true;

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line;
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", (vertex_shader_path).c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line;
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", (fragment_shader_path).c_str());
		getchar();
		exit(-1);
	}

	// Compile Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, nullptr);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		*ok = false;
	}

	// Compile Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		*ok = false;
	}

	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		*ok = false;
	}

	// free up memory
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (*ok) {
		// add uniform variable locations to cache
		ShaderUniformLocationCache::uniforms_locations[shaderProgram] = {
			{
				SHADER_UNIFORMS::mvp_matrix,
				(GLuint)glGetUniformLocation(shaderProgram, "mvp_matrix")
			},
			{
				SHADER_UNIFORMS::model,
				(GLuint)glGetUniformLocation(shaderProgram, "model")
			},
			{
				SHADER_UNIFORMS::color_type,
				(GLuint)glGetUniformLocation(shaderProgram, "color_type")
			},
			{
				SHADER_UNIFORMS::position_x,
				(GLuint)glGetUniformLocation(shaderProgram, "entity_position_x")
			},
			{
				SHADER_UNIFORMS::position_z,
				(GLuint)glGetUniformLocation(shaderProgram, "entity_position_z")
			},
			{
				SHADER_UNIFORMS::opacity,
				(GLuint)glGetUniformLocation(shaderProgram, "opacity")
			},
			{
				SHADER_UNIFORMS::worldViewPos,
				(GLuint)glGetUniformLocation(shaderProgram, "worldViewPos")
			},
			{
				SHADER_UNIFORMS::material_ambient,
				(GLuint)glGetUniformLocation(shaderProgram, "material.ambient")
			},
			{
				SHADER_UNIFORMS::material_diffuse,
				(GLuint)glGetUniformLocation(shaderProgram, "material.diffuse")
			},
			{
				SHADER_UNIFORMS::material_specular,
				(GLuint)glGetUniformLocation(shaderProgram, "material.specular")
			},
			{
				SHADER_UNIFORMS::material_shininess,
				(GLuint)glGetUniformLocation(shaderProgram, "material.shininess")
			},
			{
				SHADER_UNIFORMS::sun_direction,
				(GLuint)glGetUniformLocation(shaderProgram, "sunLight.direction")
			},
			{
				SHADER_UNIFORMS::sun_color,
				(GLuint)glGetUniformLocation(shaderProgram, "sunLight.color")
			},
			{
				SHADER_UNIFORMS::point_light_pos,
				(GLuint)glGetUniformLocation(shaderProgram, "pointLight.position")
			},
			{
				SHADER_UNIFORMS::point_light_color,
				(GLuint)glGetUniformLocation(shaderProgram, "pointLight.color")
			},
			{
				SHADER_UNIFORMS::use_texture,
				(GLuint)glGetUniformLocation(shaderProgram, "use_texture")
			},
			{
				SHADER_UNIFORMS::fog_color,
				(GLuint)glGetUniformLocation(shaderProgram, "fog_color")
			},
			{
				SHADER_UNIFORMS::daytime_value,
				(GLuint)glGetUniformLocation(shaderProgram, "daytime_value")
			},
			{
				SHADER_UNIFORMS::nighttime_value,
				(GLuint)glGetUniformLocation(shaderProgram, "nighttime_value")
			}
		};
	}

	return shaderProgram;
}

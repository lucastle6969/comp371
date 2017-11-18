#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Rock.hpp"
#include "../constants.hpp"

Rock::Rock(
	const GLuint &shader_program,
	const float& world_x_location,
	const float& world_z_location,
	const float& x_span,
	const float& z_span,
	Entity *parent
) : DrawableEntity(shader_program, parent) {
	// rectangular prism
	static const std::vector<GLuint> elements = {
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			0, 4,
			1, 5,
			2, 6,
			3, 7
	};

	this->draw_mode = GL_LINES;

	float height = 0.02f; // arbitrary height

	this->vertices = {
			glm::vec3(0.0f,   0.0f,   0.0f),   // 0: bottom-front-left
			glm::vec3(x_span, 0.0f,   0.0f),   // 1: bottom-front-right
			glm::vec3(x_span, 0.0f,   z_span), // 2: bottom-back-right
			glm::vec3(0.0f,   0.0f,   z_span), // 3: bottom-back-left
			glm::vec3(0.0f,   height, 0.0f),   // 4: top-front-left
			glm::vec3(x_span, height, 0.0f),   // 5: top-front-right
			glm::vec3(x_span, height, z_span), // 6: top-back-right
			glm::vec3(0.0f,   height, z_span)  // 7: top-back-left
	};

	this->vao = this->initVertexArray(this->vertices, elements);
}

const std::vector<glm::vec3>& Rock::getVertices() {
	return this->vertices;
}

GLuint Rock::getVAO() {
	return this->vao;
}

const int Rock::getColorType() {
	return COLOR_WHITE;
}

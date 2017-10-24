#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include "entity.hpp"
#include "worldorigin.hpp"
#include "../constants.hpp"

WorldOrigin::WorldOrigin(
	const GLuint& shader_program,
	const int& x_max,
    const int& y_max,
    const int& z_max,
	Entity* parent
) : Entity(parent)
{
	this->draw_mode = GL_LINES;
	
	// x-axis
	this->vertices.emplace_back(0.0f, 0.0f, 0.0f);
	this->vertices.emplace_back(x_max, 0.0f, 0.0f);
	// y-axis
	this->vertices.emplace_back(0.0f, 0.0f, 0.0f);
	this->vertices.emplace_back(0.0f, y_max, 0.0f);
	// z-axis
	this->vertices.emplace_back(0.0f, 0.0f, 0.0f);
	this->vertices.emplace_back(0.0f, 0.0f, z_max);

	this->vao = Entity::initVertexArray(shader_program, this->vertices);
}

const std::vector<glm::vec3>& WorldOrigin::getVertices()
{
	return this->vertices;
}

GLuint WorldOrigin::getVAO()
{
	return this->vao;
}

const int WorldOrigin::getColorType()
{
	return COLOR_COORDINATE_AXES;
}
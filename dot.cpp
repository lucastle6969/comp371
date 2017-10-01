#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include "entity.hpp"
#include "dot.hpp"
#include "objloader.hpp"
#include "constants.hpp"

Dot::Dot(const GLuint& shader_program, Entity* parent) : Entity(parent)
{
	this->draw_mode = GL_TRIANGLES;

	// for now we can read but then ignore the normals and UVs
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;

	// get the vertices from the sphere.obj file (ignore the rest)
	loadOBJ("../sphere.obj", &this->vertices, &normals, &UVs);
	this->vao = Entity::initVertexArray(shader_program, this->vertices);
}

const std::vector<glm::vec3>& Dot::getVertices()
{
	return this->vertices;
}

GLuint Dot::getVAO()
{
	return this->vao;
}

const int Dot::getColorType()
{
	return COLOR_TURQUOISE;
}

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "entity.hpp"
#include "pacman.hpp"
#include "objloader.hpp"
#include "constants.hpp"

Pacman::Pacman(const GLuint& shader_program, Entity* parent) : Entity(parent)
{
	this->draw_mode = GL_TRIANGLES;

	// for now we can read but then ignore the normals and UVs
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;

	// get the vertices from the pacman.obj file (ignore the rest)
	loadOBJ("../pacman.obj", &this->pacman_vertices, &normals, &UVs);
	this->pacman_vao = Entity::initVertexArray(shader_program, this->pacman_vertices);

	// get the vertices from the teapot.obj file (ignore the rest)
	loadOBJ("../teapot.obj", &this->teapot_vertices, &normals, &UVs);
	this->teapot_vao = Entity::initVertexArray(shader_program, this->teapot_vertices);

	this->using_teapot = false;
}

const std::vector<glm::vec3>& Pacman::getVertices()
{
	return this->using_teapot ? this->teapot_vertices : this->pacman_vertices;
}

GLuint Pacman::getVAO()
{
	return this->using_teapot ? this->teapot_vao : this->pacman_vao;
}

const int Pacman::getColorType()
{
	return COLOR_WHITE;
}

// returns new using_teapot bool
bool Pacman::toggleTeapot()
{
	// Toggle using_teapot
	this->using_teapot = !this->using_teapot;

	// Adjust model scale to match new model
	this->scale(this->using_teapot ? Pacman::teapot_scale_ratio : 1.0f / Pacman::teapot_scale_ratio);

	return this->using_teapot;
}

const glm::mat4& Pacman::getBaseRotation()
{
	static glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	static glm::mat4 identity;
	static glm::mat4 pacman = glm::rotate(identity, 0.0f - Pacman::teapot_rotation_angle, x_axis);
	static glm::mat4 teapot = identity;

	// Return appropriate rotation offset depending on which model we're using
	return this->using_teapot ? teapot : pacman;
}

// the vector indicating the direction the model faces by default (with no rotation)
const glm::vec3& Pacman::getDefaultFaceVector()
{
	static glm::vec3 new_face_vec(1.0f, 0.0f, 0.0f);

	return new_face_vec;
}

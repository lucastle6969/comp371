#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Player.hpp"
#include "../objloader.hpp"
#include "../constants.hpp"

Player::Player(const GLuint& shader_program, Entity* parent) : DrawableEntity(shader_program, parent)
{
	this->draw_mode = GL_TRIANGLES;

	// for now we can read but then ignore the normals and UVs
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	std::vector<GLuint> elements;

	// get the vertices from the pacman.obj file (ignore the rest)
	loadOBJ("../models/pacman.obj", &this->vertices, &normals, &UVs, &elements);
	//this->vao = this->initVertexArray(this->vertices, elements);
	this->vao = this->initVertexArray(this->vertices, elements, UVs, normals, glm::vec3(1,10,1), glm::vec3(.10,.10,.10), glm::vec3(.25,.25,.25), .25);

}

const std::vector<glm::vec3>& Player::getVertices()
{
	return this->vertices;
}

GLuint Player::getVAO()
{
	return this->vao;
}

const int Player::getColorType()
{
	return COLOR_WHITE;
}

const glm::mat4& Player::getBaseRotation()
{
	static glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	static glm::mat4 identity;
	static glm::mat4 rotation = glm::rotate(identity, Player::base_rotation_angle, x_axis);

	return rotation;
}

// the vector indicating the direction the model faces by default (with no rotation)
const glm::vec3& Player::getDefaultFaceVector()
{
	static glm::vec3 default_face_vec(1.0f, 0.0f, 0.0f);

	return default_face_vec;
}

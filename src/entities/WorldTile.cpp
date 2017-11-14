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
#include "WorldTile.hpp"

#include "../constants.hpp"


WorldTile::WorldTile(
	const GLuint &shader_program,
	const int& x_location,
	const int& z_location,
	Entity *parent
) : DrawableEntity(shader_program, parent)
{
	this->draw_mode = GL_TRIANGLES;

	// position tile relative to parent based on x, z inputs
	this->translate(glm::vec3(x_location, 0.0f, z_location));
}

const std::vector<glm::vec3>& WorldTile::getVertices() {
	static const std::vector<glm::vec3> vertices = {
			glm::vec3(0.0f, 0.0f, -1.0f), // bottom-left
			glm::vec3(1.0f, 0.0f, -1.0f), // bottom-right
			glm::vec3(1.0f, 0.0f, 0.0f), // top-right
			glm::vec3(0.0f, 0.0f, 0.0f)  // top-left
	};

	return vertices;
}

GLuint WorldTile::getVAO() {
	static const std::vector<GLuint> elements = {
			// first triangle (ACTUALLY is counterclockwise - negative-Z axis)
			3, // top-left
			1, // bottom-right
			0, // bottom-left
			// second triangle
			3, // top-left
			2, // top-right
			1  // bottom-right
	};

	static GLuint vao;
	static bool vao_init = false;

	if (!vao_init) {
		// only initialize vao once for all instances
		vao = this->initVertexArray(this->getVertices(), elements);
		vao_init = true;
	}

	return vao;
}

const int WorldTile::getColorType() {
	return COLOR_TILE;
}

const glm::mat4 &WorldTile::getBaseRotation() {
	static glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	static glm::mat4 identity;
	static glm::mat4 rotation = glm::rotate(identity, WorldTile::base_rotation_angle, x_axis);

	return rotation;
}

// the vector indicating the direction the model faces by default (with no rotation)
const glm::vec3 &WorldTile::getDefaultFaceVector() {
	static glm::vec3 default_face_vec(1.0f, 0.0f, 0.0f);

	return default_face_vec;
}

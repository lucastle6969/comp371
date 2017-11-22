#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Rock.hpp"
#include "Tree.hpp"
#include "WorldTile.hpp"
#include "../utils.hpp"
#include "../constants.hpp"

WorldTile::WorldTile(
	const GLuint &shader_program,
	const int& world_x_location,
	const int& world_z_location,
	Entity *parent
) : DrawableEntity(shader_program, parent)
{
	this->draw_mode = GL_TRIANGLES;

	// grass
	this->setMaterial(
		glm::vec3(0.34f, 0.49f, 0.27f),
		glm::vec3(0.34f, 0.49f, 0.27f),
		glm::vec3(0.0f),
		0.01f
	);

	// position tile relative to parent based on x, z inputs
	this->translate(glm::vec3(world_x_location, 0.0f, world_z_location));

	// initialize random number generator based on world location
	srand((unsigned int)(world_x_location * world_z_location + world_x_location + world_z_location));

	// TODO: better tree/rock distribution?
	// TODO: test/remove tree/rock overlaps

	// add rocks
	for (int i = 0; i < 10; i++) {
		float x_span = utils::randomFloat(0.02f, 0.05f);
		float z_span = utils::randomFloat(0.02f, 0.05f);
		float x_position = utils::randomFloat(0.0f, 1.0f - x_span);
		float z_position = utils::randomFloat(0.0f, 1.0f - z_span);
		// Add rock child
		Rock* rock = new Rock(
				shader_program,
				world_x_location + x_position,
				world_z_location + z_position,
				x_span,
				z_span,
				this
		);
		rock->setPosition(glm::vec3(x_position, 0.0f, z_position));
        rock->scale(0.01f);
		// Add rock to rocks array
		this->rocks.emplace_back(rock);
	}

	// add trees
	for (int i = 0; i < 10; i++) {
		float base_span = utils::randomFloat(0.02f, 0.05f);
		float x_position = utils::randomFloat(0.0f, 1.0f - base_span);
		float z_position = utils::randomFloat(0.0f, 1.0f - base_span);
		// Add tree child
		Tree* tree = new Tree(
				shader_program,
				world_x_location + x_position,
				world_z_location + z_position,
				base_span,
				this
		);
		tree->setPosition(glm::vec3(x_position, 0.0f, z_position));
		// Add tree to trees array
		this->trees.emplace_back(tree);
	}
}

WorldTile::~WorldTile()
{
	for (Rock* const& rock : this->rocks) {
		delete rock;
	}
	for (Tree* const& tree : this->trees) {
		delete tree;
	}
}

const std::vector<glm::vec3>& WorldTile::getVertices() {
	static const std::vector<glm::vec3> vertices = {
			glm::vec3(0.0f, 0.0f, 0.0f), // bottom-left
			glm::vec3(1.0f, 0.0f, 0.0f), // bottom-right
			glm::vec3(1.0f, 0.0f, 1.0f), // top-right
			glm::vec3(0.0f, 0.0f, 1.0f)  // top-left
	};

	return vertices;
}

GLuint WorldTile::getVAO() {
	static const std::vector<GLuint> elements {
			// first triangle (ACTUALLY is counterclockwise - negative-Z axis)
			3, // top-left
			1, // bottom-right
			0, // bottom-left
			// second triangle
			3, // top-left
			2, // top-right
			1  // bottom-right
	};

	static const std::vector<glm::vec3> normals {
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
	};

	static GLuint vao;
	static bool vao_init = false;

	if (!vao_init) {
		// only initialize vao once for all instances
		vao = this->initVertexArray(
				this->getVertices(),
				elements,
				normals
		);
		vao_init = true;
	}

	return vao;
}

const int WorldTile::getColorType() {
	return COLOR_LIGHTING;
}

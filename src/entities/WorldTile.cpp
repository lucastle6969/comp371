#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Rock.hpp"

#include "src/entities/Trees/Tree.hpp"
#include "src/Entities/Trees/TreeA.hpp"
#include "src/Entities/Trees/TreeB.hpp"
#include "src/Entities/Trees/TreeC.hpp"

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

	this->setMaterial(
		glm::vec3(.5,.5,.5), // need to change this to some other value... maybe the height of the plane if we ever make it.
		glm::vec3(.5,.5,.5),
		glm::vec3(.25,.25,.25),
		0.25f
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

	//enable tree distributor function
	//TreeDistributor::setEntity(this);
	// add trees
	for (int i = 0; i < 5; i++) {
		static const float scale_factor = 100;
		float base_span = utils::randomFloat(0.02f, 0.05f);
		float internal_tree_width = base_span * scale_factor;
		float x_position = utils::randomFloat(0.0f, 1.0f - base_span);
		float z_position = utils::randomFloat(0.0f, 1.0f - base_span);
		int seed = std::abs((world_x_location + x_position) * (world_z_location + z_position))*scale_factor;
		// Add tree child
		Tree* tree;
		if(seed % 10 < 2){
			tree = new TreeA(shader_program, this, internal_tree_width*2.5, seed);
		}
		else if(seed % 10 < 7){
			tree = new TreeB(shader_program, this, internal_tree_width, seed);
		}
		else{
			tree = new TreeC(seed % 15, shader_program, this, internal_tree_width, seed);
		}
//		std::cout << x_position << " " <<  z_position <<" " << tree->getType() << "\n";
		tree->setPosition(glm::vec3(x_position, 0.0f, z_position));
		tree->scale(1.0f / (scale_factor*10));
		// Add tree to trees array
		this->trees.emplace_back(tree);
	}
//	printf("=====================\n");
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
	return COLOR_TILE;
}

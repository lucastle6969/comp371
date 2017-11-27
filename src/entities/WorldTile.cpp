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

#include <src/utils.hpp>
#include <src/constants.hpp>
#include <src/HitBox2d.hpp>

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Rock.hpp"

#include "src/entities/Trees/Tree.hpp"
#include "src/entities/Trees/TreeA.hpp"
#include "src/entities/Trees/TreeB.hpp"
#include "src/entities/Trees/TreeC.hpp"

#include "WorldTile.hpp"

WorldTile::WorldTile(
	const GLuint &shader_program,
	const int& world_x_location,
	const int& world_z_location,
	const float& min_hitbox_y,
	const float& max_hitbox_y,
	const HitBox2d& player_hitbox,
	Entity *parent
) : DrawableEntity(shader_program, parent)
{
	this->draw_mode = GL_TRIANGLES;

	this->setMaterial(
		glm::vec3(.5,.5,.5), // need to change this to some other value... maybe the height of the plane if we ever make it.
		glm::vec3(.5,.5,.5),
		glm::vec3(.25,.25,.25),
		25.0f
	);

	// position tile relative to parent based on x, z inputs
	this->translate(glm::vec3(world_x_location, 0.0f, world_z_location));

	// initialize random number generator based on world location
	srand((unsigned int)(world_x_location * world_z_location + world_x_location + world_z_location));

	// TODO: better tree/rock distribution?
	// TODO: test/remove tree/rock overlaps

	// add rocks
	int rock_tries_left = 3;
	for (int i = 0; i < 7; i++) {
		float x_span = utils::randomFloat(0.02f, 0.05f);
		float z_span = utils::randomFloat(0.02f, 0.05f);
        float y_span = utils::randomFloat(0.005f, 0.015f);
		float x_position = utils::randomFloat(0.0f, 1.0f - x_span);
		float z_position = utils::randomFloat(0.0f, 1.0f - z_span);

		HitBox2d box(x_position, z_position, x_position + x_span, z_position + z_span);
		if (this->collidesWith(box) || player_hitbox.collidesWith(box)) {
			if (rock_tries_left-- > 0) {
				// collision! but give it another try
				i--;
			}
			continue;
		}

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

        rock->scale(glm::vec3(x_span, y_span, z_span));
		// Add rock to rocks array
		this->rocks.emplace_back(rock);
		this->hitboxes.emplace_back(*rock, min_hitbox_y, max_hitbox_y);
		std::cout << "ROCK:" << std::endl;
		std::cout << "This is what I gave you... x: " << x_position << " z: " << z_position << " x-span: " << x_span << " z-span: " << z_span << std::endl;
		std::cout << "And this is what you gave me: " << this->hitboxes[this->hitboxes.size() - 1] << std::endl;
	}

	int rockb_tries_left = 3;
    for (int i = 0; i < 7; i++) {
        float x_span = utils::randomFloat(0.02f, 0.05f);
        float z_span = utils::randomFloat(0.02f, 0.05f);
        float y_span = utils::randomFloat(0.005f, 0.025f);
        float x_position = utils::randomFloat(0.0f, 1.0f - x_span);
        float z_position = utils::randomFloat(0.0f, 1.0f - z_span);

	    HitBox2d box(x_position, z_position, x_position + x_span, z_position + z_span);
	    if (this->collidesWith(box) || player_hitbox.collidesWith(box)) {
		    if (rockb_tries_left-- > 0) {
			    // collision! but give it another try
			    i--;
		    }
		    continue;
	    }

        // Add rock child
        RockB* rock = new RockB(
                shader_program,
                world_x_location + x_position,
                world_z_location + z_position,
                x_span,
                z_span,
                this
        );
        rock->setPosition(glm::vec3(x_position, 0.002f, z_position));

        rock->scale(glm::vec3(x_span, y_span, z_span));
        // Add rock to rocks array
        this->rocksB.emplace_back(rock);
	    this->hitboxes.emplace_back(*rock, min_hitbox_y, max_hitbox_y);
	    std::cout << "ROCK B:" << std::endl;
	    std::cout << "This is what I gave you... BL: (" << x_position << ", " << z_position << "), TR: (" << x_position + x_span << ", " << z_position + z_span << ")" << std::endl;
	    std::cout << "And this is what you gave me: " << this->hitboxes[this->hitboxes.size() - 1] << std::endl;
    }

	//enable tree distributor function
	//TreeDistributor::setEntity(this);
	// add trees
	int tree_tries_left = 2;
	for (int i = 0; i < 5; i++) {
		static const float scale_factor = 100;
		float base_span = utils::randomFloat(0.02f, 0.05f);
		float internal_tree_width = base_span * scale_factor;
		float x_position = utils::randomFloat(0.0f, 1.0f - base_span);
		float z_position = utils::randomFloat(0.0f, 1.0f - base_span);

		HitBox2d box(x_position, z_position, x_position + base_span, z_position + base_span);
		if (this->collidesWith(box) || player_hitbox.collidesWith(box)) {
			if (tree_tries_left-- > 0) {
				// collision! but give it another try
				i--;
			}
			continue;
		}

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
		this->hitboxes.emplace_back(*tree, min_hitbox_y, max_hitbox_y);
		std::cout << "TREE:" << std::endl;
		std::cout << "This is what I gave you... x: " << x_position << " z: " << z_position << " base-span: " << base_span << std::endl;
		std::cout << "And this is what you gave me: " << this->hitboxes[this->hitboxes.size() - 1] << std::endl;
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

    for (RockB* const& rock : this->rocksB) {
        delete rock;
    }
}

const std::vector<glm::vec3>& WorldTile::getVertices() const
{
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

// tests for x-z collision between the specified box and any child entities.
// does NOT test if the box exists at all in the tile space.
bool WorldTile::collidesWith(const HitBox2d &box) const
{
	for (const HitBox2d& hb : this->hitboxes) {
		if (hb.collidesWith(box)) {
			return true;
		}
	}
	return false;
}

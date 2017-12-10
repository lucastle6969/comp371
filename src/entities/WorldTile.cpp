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
#include "PerlinTerrain.hpp"
#include "Rock.hpp"
#include "src/entities/Trees/Tree.hpp"
#include "src/entities/Trees/TreeA.hpp"
#include "src/entities/Trees/TreeA_Autumn.hpp"
#include "src/entities/Trees/TreeB.hpp"
#include "src/entities/Trees/TreeCluster.hpp"
#include "WorldTile.hpp"
#include "Tentacle.hpp"


WorldTile::WorldTile(
	const GLuint& shader_program,
	const int& world_x_location,
	const int& world_z_location,
	const float& base_min_hitbox_y,
	const float& base_max_hitbox_y,
	const HitBox2d& player_hitbox,
	Entity *parent
) : Entity(parent),
    water(shader_program, this),
    seed_loc_message(
		    shader_program,
		    "Seed for current location: " + std::to_string(world_x_location) + ':' + std::to_string(world_z_location),
		    0,
		    0,
		    FONT_STYLE_OUTLINE,
		    this
    )
{
	// position tile relative to parent based on x, z inputs
	this->translate(glm::vec3(world_x_location, 0.0f, world_z_location));

	int terrain_span = WorldTile::terrain_width;
	this->terrain = new PerlinTerrain(
			shader_program,
			world_x_location,
			world_z_location,
			terrain_span,
			terrain_span,
			this
	);
	// terrain height is from -1 to 1 internally
	glm::vec3 terrain_scale(1.0f, VERTICAL_TERRAIN_SCALE, 1.0f);
	this->terrain->scale(terrain_scale);

	this->water.setPosition(glm::vec3(0.0f, WATER_ELEVATION, 0.0f));

	glm::vec3 terrain_loc_under_seed_loc_message =
			this->terrain->findIntersectionPoint(0, 0) * terrain_scale;
	if (terrain_loc_under_seed_loc_message.y < WATER_ELEVATION) {
		terrain_loc_under_seed_loc_message.y = WATER_ELEVATION;
	}

	this->seed_loc_message.setPosition(
			terrain_loc_under_seed_loc_message + glm::vec3(0.0f, -0.95, 0.0f)
	);

	// initialize random number generator based on world location
	srand((unsigned int)(world_x_location * world_z_location + world_x_location + world_z_location));

	// TODO: better tree/rock distribution?
	// TODO: test/remove tree/rock overlaps

	static const glm::vec3 rock_a_offset(0.0f, -0.004f, 0.0f);
	static const glm::vec3 rock_b_offset(0.0f, 0.0f, 0.0f);

	static const int max_rocks_general = 7;
	static const int max_rocks_alien = 14;
	static const int max_rocks_tentacle = 14;
	static const int max_rocks_forest = 14;

	static const int max_trees = 5;

    static const int general_biome_max_x = 5, general_biome_max_z = 5;
    static const int alien_biome_max_x = 5, alien_biome_max_z = 10;
    static const int tentacle_biome_max_x = 10, tentacle_biome_max_z = 5;
    static const int worldBoundaries = 10;

	static const int GENERAL_BIOME = 0;
	static const int ALIEN_BIOME = 1;
	static const int TENTACLE_BIOME = 2;
	static const int FOREST_BIOME = 3;

	int current_biome = FOREST_BIOME;
	int max_rocks = max_rocks_forest;
	{
		int b_x = abs(world_x_location) % worldBoundaries;
		int b_z = abs(world_z_location) % worldBoundaries;
		if (b_x < general_biome_max_x && b_z < general_biome_max_z) {
			current_biome = GENERAL_BIOME;
			max_rocks = max_rocks_general;
		} else if (b_x < alien_biome_max_x && b_z < alien_biome_max_z) {
			current_biome = ALIEN_BIOME;
			max_rocks = max_rocks_alien;
		} else if (b_x < tentacle_biome_max_x && b_z < tentacle_biome_max_z) {
			current_biome = TENTACLE_BIOME;
			max_rocks = max_rocks_tentacle;
		}
	}

	// add rocks
	int rock_tries_left = max_rocks / 2;
	for (int i = 0; i < max_rocks; i++) {
		float x_span = utils::randomFloat(0.02f, 0.05f);
		float z_span = utils::randomFloat(0.02f, 0.05f);
        float y_span = utils::randomFloat(0.01f, 0.015f);
		float x_position = utils::randomFloat(0.0f + x_span / 2, 1.0f - x_span / 2);
		float z_position = utils::randomFloat(0.0f + z_span / 2, 1.0f - z_span / 2);

		HitBox2d box(
				world_x_location + x_position - x_span / 2,
				world_z_location + z_position - z_span / 2,
				world_x_location + x_position + x_span / 2,
				world_z_location + z_position + z_span / 2
		);
		if (this->collidesWith(box) || player_hitbox.collidesWith(box)) {
			if (rock_tries_left-- > 0) {
				// collision! but give it another try
				i--;
			}
			continue;
		}

		bool use_rock_b = (int)(ceil(x_position * y_span + world_z_location)) % 2 == 0;

		DrawableEntity* rock;
		if (use_rock_b) {
			rock = new RockB(
					shader_program,
					world_x_location + x_position,
					world_z_location + z_position,
					x_span,
					z_span,
					this
			);
		} else {
			rock = new Rock(
					shader_program,
					world_x_location + x_position,
					world_z_location + z_position,
					x_span,
					z_span,
					this
			);
		}

		rock->scale(glm::vec3(x_span, y_span, z_span));
		glm::vec3 terrain_intersection =
				this->terrain->findIntersectionPoint(x_position, z_position) *
				terrain_scale;
		rock->setPosition(
				terrain_intersection + (use_rock_b ? rock_b_offset : rock_a_offset)
		);
		// Add rock to rocks array
		if (use_rock_b) {
			this->rocksB.push_back((RockB*)rock);
		} else {
			this->rocks.push_back((Rock*)rock);
		}
		this->hitboxes.emplace_back(
				*rock,
				base_min_hitbox_y + terrain_intersection.y,
				base_max_hitbox_y + terrain_intersection.y
		);
	}

	// add trees
	int tree_tries_left = max_trees / 2;
	for (int i = 0; i < max_trees; i++) {
		static const float scale_factor = 100;
		float base_span = utils::randomFloat(0.02f, 0.05f);
		float internal_tree_width = base_span * scale_factor;
		float x_position = utils::randomFloat(0.0f + base_span / 2, 1.0f - base_span / 2);
		float z_position = utils::randomFloat(0.0f + base_span / 2, 1.0f - base_span / 2);

		HitBox2d box(
				world_x_location + x_position - base_span / 2,
				world_z_location + z_position - base_span / 2,
				world_x_location + x_position + base_span / 2,
				world_z_location + z_position + base_span / 2
		);
		if (this->collidesWith(box) || player_hitbox.collidesWith(box)) {
			if (tree_tries_left-- > 0) {
				// collision! but give it another try
				i--;
			}
			continue;
		}

		int seed = std::abs((world_x_location + x_position) * (world_z_location + z_position))*scale_factor;
		seed = seed == 0 ? (world_x_location + x_position + world_z_location + z_position+3 )* 7: seed;

		float tree_magnitude = 1.0f / (scale_factor * 10);

		// Add tree children
        std::vector<Tree*> new_trees;

        if (current_biome == GENERAL_BIOME) {
            bool isAlien = false;
            if (seed % worldBoundaries < 2) {
                if (seed % 2 == 0)
                    new_trees.push_back(new TreeA(shader_program, this, internal_tree_width * 2.5, seed, isAlien));
                else
                    new_trees.push_back(new TreeA_Autumn(shader_program, this, internal_tree_width * 2.5, seed));
            } else if (seed % worldBoundaries < 7) {
                new_trees.push_back(new TreeB(shader_program, this, internal_tree_width, seed, isAlien));
            } else {
	            TreeCluster::generateCluster(
			            &new_trees,
			            this,
			            seed,
			            seed % 15,
			            shader_program,
			            internal_tree_width,
			            isAlien,
			            tree_magnitude
	            );
            }
        } else if (current_biome == ALIEN_BIOME) {
            bool isAlien = true;
            if (seed % worldBoundaries < 2) {
	            new_trees.push_back(new TreeA(shader_program, this, internal_tree_width * 2.5, seed, isAlien));
            } else if (seed % worldBoundaries < 7) {
                new_trees.push_back(new TreeB(shader_program, this, internal_tree_width, seed, isAlien));
            } else {
	            TreeCluster::generateCluster(
			            &new_trees,
			            this,
			            seed,
			            seed % 15,
			            shader_program,
			            internal_tree_width,
			            isAlien,
			            tree_magnitude
	            );
            }
        } else if (current_biome == TENTACLE_BIOME) {
            bool isAlien, isTextured;
            if(seed % 3 == 0) {
                isAlien = false, isTextured = true;
            }
            else if(seed % 3 == 1)
               isAlien = true, isTextured = true;
            else
                isAlien = true, isTextured = false;

            new_trees.push_back(new Tentacle(shader_program, this, internal_tree_width * 2.5, seed, isAlien, isTextured));
        } else /* if (current_biome == FOREST_BIOME) */ {
            bool isAlien = false;
            if (seed % 10 < 7) {
                if (seed % 2 == 0)
                    new_trees.push_back(new TreeA(shader_program, this, internal_tree_width * 3, seed, isAlien));
                else
                    new_trees.push_back(new TreeA_Autumn(shader_program, this, internal_tree_width * 3, seed));
            } else {
	            TreeCluster::setSpacingConstant(10);
	            TreeCluster::generateCluster(
			            &new_trees,
			            this,
			            seed,
			            0,
			            shader_program,
			            internal_tree_width * 1.5f,
			            isAlien,
			            tree_magnitude
	            );
	            TreeCluster::setSpacingConstant(10);
            }
        }

		for (Tree* tree : new_trees) {
			tree->scale(tree_magnitude);
			// use translate instead of setPosition so we can take
			// into account any prior offsets from tree clustering
			tree->translate(glm::vec3(x_position, 0.0f, z_position));
			const glm::vec3& tree_pos = tree->getPosition();
			glm::vec3 terrain_intersection =
					this->terrain->findIntersectionPoint(tree_pos.x, tree_pos.z) *
					terrain_scale;
			if (terrain_intersection.y < WATER_ELEVATION
					|| tree_pos.x < 0
					|| tree_pos.x >= 1
					|| tree_pos.z < 0
					|| tree_pos.z >= 1
			) {
				// we don't want trees to spawn underwater.
				// we also can't handle trees that don't sit on the tile
				// (this might happen for tree cluster items)
				this->detachChild(tree);
				delete tree;
				continue;
			}
			tree->setPosition(
					terrain_intersection -
					// dip tree into ground proportionally to its height
					glm::vec3(0.0f, tree->getWorldHeight() / 30.0f, 0.0f)
			);
			// Add tree to trees array
			this->trees.emplace_back(tree);
			this->hitboxes.emplace_back(
					*tree,
					base_min_hitbox_y + terrain_intersection.y,
					base_max_hitbox_y + terrain_intersection.y
			);
		}
	}
}

WorldTile::~WorldTile()
{
	delete this->terrain;
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

const PerlinTerrain& WorldTile::getTerrain() const
{
	return *this->terrain;
}

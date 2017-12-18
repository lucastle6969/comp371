#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <algorithm>

#include <src/HitBox2d.hpp>

#include "World.hpp"
#include "../constants.hpp"

World::World(
	const GLuint& shader_program,
	const float& player_x_start,
	const float& player_z_start,
	Entity* parent
) : Entity(parent),
    shader_program(shader_program),
    player(shader_program, this),
    menu(shader_program, "modulus woods\n\nAndre Marques Manata\nBen Wiley\nKai Nicoll-Griffith\nCarlo Gentile\nEmile Aoun", 0, 0, FONT_STYLE_MYTHOS, this),
    axes(shader_program, WORLD_X_MAX, WORLD_X_MAX, WORLD_Z_MAX, this),
	x_center((int)floor(player_x_start)),
	z_center((int)floor(player_z_start)),
    player_base_min_world_y(FLT_MAX),
    player_base_max_world_y(-FLT_MAX),
    handling_player_knockback(false)
{
    // hide the axes by default
	this->axes.hide();

	this->player.scale(0.0005f);
	this->player.setPosition(glm::vec3(player_x_start, 0.01f, player_z_start));

	// assume player's floating position from ground will remain constant and use
	// the y range of the model to create specialized entity hit boxes for collision
	// detection
	glm::mat4 player_model_matrix = player.getModelMatrix();
	for (const glm::vec3& vertex : this->player.getVertices()) {
        float y = (player_model_matrix * glm::vec4(vertex, 1.0f)).y;
        this->player_base_min_world_y = std::min(this->player_base_min_world_y, y);
        this->player_base_max_world_y = std::max(this->player_base_max_world_y, y);
	}
	HitBox2d player_starting_hitbox(this->player);
	// populate tiles
	int x, z;
	for (int i = 0; i < 9; i++) {
		World::tileIndexToLocation(i, this->x_center, this->z_center, &x, &z);
		// create tile and add to list of tiles AS WELL AS list of children
		this->tiles.push_back(new WorldTile(
				shader_program,
				x,
				z,
				this->player_base_min_world_y,
				this->player_base_max_world_y,
				player_starting_hitbox,
				this
		));
	}

	// place player relative to terrain
	std::cout << "player pos orig " << this->player.getPosition().x << ", " << this->player.getPosition().y << ", " << this->player.getPosition().z << std::endl;
	this->player.setPosition(
			this->getSurfacePoint(player_x_start, player_z_start) +
					glm::vec3(0.0f, 0.01f, 0.0f)
	);
	std::cout << "player pos new " << this->player.getPosition().x << ", " << this->player.getPosition().y << ", " << this->player.getPosition().z << std::endl;
}

World::~World()
{
	for (WorldTile* const& tile : this->tiles) {
		delete tile;
	}
}

const Player* World::getPlayer()
{
	return &this->player;
}

Text* World::getMenu() {
	return &this->menu;
}

void World::toggleAxes()
{
	this->axes.toggleHide();
}

void World::checkPosition()
{
	static const bool debug_position = false;

	glm::vec3 position = this->player.getPosition();
	if (debug_position) {
		std::cout << "position";
		std::cout << " x " << position.x;
		std::cout << " y " << position.y;
		std::cout << " z " << position.z;
		std::cout << std::endl;
	}

	HitBox2d player_current_hitbox(this->player);

	int diff_x = (int)floor(position.x) - this->x_center;
	int diff_z = (int)floor(position.z) - this->z_center;

	// just to be safe make sure the value is either 1 or -1
	if (diff_x != 0) diff_x = diff_x > 0 ? 1 : -1;
	if (diff_z != 0) diff_z = diff_z > 0 ? 1 : -1;
	// TODO: maybe account for cases where player has moved more than one tile?

	// swap in new column of tiles if needed
	if (diff_x != 0) {
		int new_col_x = this->x_center + diff_x * 2;
		for (int z = this->z_center - 1; z <= this->z_center + 1; z++) {
			this->placeWorldTile(new_col_x, z, player_current_hitbox);
		}
		this->x_center += diff_x;
	}

	// swap in new row of tiles if needed
	if (diff_z != 0) {
		int new_row_z = this->z_center + diff_z * 2;
		for (int x = this->x_center - 1; x <= this->x_center + 1; x++) {
			this->placeWorldTile(x, new_row_z, player_current_hitbox);
		}
		this->z_center += diff_z;
	}
}

void World::placeWorldTile(const int &x, const int &z, const HitBox2d& player_hitbox)
{
	// find tile array index corresponding to new tile location
	int index = World::locationToTileIndex(x, z);
	// get pointer to existing tile occupying this space
	WorldTile* old_tile = this->tiles[index];
	// remove old tile pointer from list of children
	this->detachChild(old_tile);
	// free old tile
	delete old_tile;
	// create new tile, add it to list of children, and replace old tile in tiles array
	this->tiles[index] = new WorldTile(
			this->shader_program,
			x,
			z,
			this->player_base_min_world_y,
			this->player_base_max_world_y,
			player_hitbox,
			this
	);
}

int World::locationToTileIndex(const int& x, const int& z)
{
	// NOTE: Mapping position -> tile index is arbitrary, but importantly, consistent!
	// Just like x % 3 except the same pattern continues for negative numbers
	// (3 -> 0, 2 -> 2, 1 -> 1, 0 -> 0, -1 -> 2, -2 -> 1, -3 -> 0, etc)
	int x_index = x - int(floor(x / 3.0f)) * 3;
	int z_index = z - int(floor(z / 3.0f)) * 3;
	return z_index * 3 + x_index;
}

void World::tileIndexToLocation(
	const int& index,
	const int& x_center,
	const int& z_center,
	int* const& x,
	int* const& z
) {
	// TODO: more efficient solution (although this function only gets run once per World instance)
	for (int _x = x_center - 1; _x <= x_center + 1; _x++) {
		for (int _z = z_center - 1; _z <= z_center + 1; _z++) {
			if (index == World::locationToTileIndex(_x, _z)) {
				*x = _x;
				*z = _z;
				return;
			}
		}
	}
	throw std::runtime_error("Tile index should have matching location.");
}

void World::setPlayerOpacity(const float& opacity)
{
	this->player.setOpacity(opacity);
}

void World::movePlayer(const glm::vec3& move_vec, const float& units)
{
	glm::vec3 old_player_position = this->player.getPosition();
	HitBox2d player_hitbox(this->player);
	this->player.move(move_vec, units);
	if (this->collidesWith(player_hitbox)) {
		this->player.setPosition(old_player_position);
		this->handling_player_knockback = true;
		glm::vec3 knockback_target =
				old_player_position - 4.0f * units * glm::normalize(move_vec);
		this->player_knockback_target_2d = glm::vec2(knockback_target.x, knockback_target.z);
	} else {
		this->checkPosition();
		glm::vec3 pos = this->player.getPosition();
		this->player.setPosition(
				this->getSurfacePoint(
						pos.x,
						pos.z
				) + glm::vec3(0.0f, 0.01f, 0.0f)
		);
	}
}

bool World::collidesWith(const HitBox2d& box)
{
	for (const WorldTile* tile : this->tiles) {
		if (tile->collidesWith(box)) {
			return true;
		}
	}
	return false;
}

glm::vec3 World::getSurfacePoint(const float &x, const float &z)
{
	WorldTile* center = this->tiles[
			World::locationToTileIndex(this->x_center, this->z_center)
	];
	const PerlinTerrain& terrain = center->getTerrain();
	glm::vec4 world_terrain_intersection_point =
			terrain.getModelMatrix()
			* glm::vec4(terrain.findIntersectionPoint(
					x - this->x_center,
					z - this->z_center
			), 1.0f);
	// return the water surface point if the terrain is below water
	return glm::vec3(
			world_terrain_intersection_point.x,
			std::max(world_terrain_intersection_point.y, WATER_ELEVATION),
			world_terrain_intersection_point.z
	);
}

bool World::handlePlayerKnockback()
{
	if (!this->handling_player_knockback) {
		return false;
	}
	glm::vec3 player_position = this->player.getPosition();

	// diminish knockback rate over time to simulate friction
	glm::vec2 knockback_vec_2d = (
			this->player_knockback_target_2d
			- glm::vec2(player_position.x, player_position.z)
	) / 3.0f;

	// knock back player
	this->player.translate(glm::vec3(knockback_vec_2d[0], 0.0f, knockback_vec_2d[1]));

	// check if we're close enough to the target to stop knocking back
	glm::vec3 player_new_position = this->player.getPosition();
	float distance_left = glm::length(
			this->player_knockback_target_2d
			- glm::vec2(player_new_position.x, player_new_position.z)
	);
	if (distance_left < 0.001f) {
		// close enough
		this->handling_player_knockback = false;
	}

	// adjust player y to sit on terrain
	this->player.setPosition(
			this->getSurfacePoint(
					player_new_position.x,
					player_new_position.z
			) + glm::vec3(0.0f, 0.01f, 0.0f)
	);

	return true;
}

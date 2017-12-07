#ifndef PROCEDURALWORLD_WORLD_HPP
#define PROCEDURALWORLD_WORLD_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include <src/HitBox2d.hpp>

#include "Light.h"
#include "Entity.hpp"
#include "Player.hpp"
#include "WorldOrigin.hpp"
#include "WorldTile.hpp"
#include "Text.hpp"

class World: public Entity {
private:
	static int locationToTileIndex(const int& x, const int& z);
	static void tileIndexToLocation(
		const int& index,
		const int& x_center,
		const int& z_center,
		int* const& x,
		int* const& z
	);
	Player player;
	WorldOrigin axes;
    Text menu;
	std::vector<WorldTile*> tiles;
	// current center tile position
	int x_center;
	int z_center;
	GLuint shader_program;
	float player_base_min_world_y;
	float player_base_max_world_y;
	bool handling_player_knockback;
	glm::vec3 player_knockback_target;
	bool collidesWith(const HitBox2d& box);
	void placeWorldTile(const int& x, const int& z, const HitBox2d& player_hitbox);
	void checkPosition();
	glm::vec3 getTerrainIntersectionPoint(const float& x, const float& z);
public:
	explicit World(const GLuint& shader_program) : World(shader_program, nullptr) {}
	World(const GLuint& shader_program, Entity* parent)
		: World(shader_program, 0.0f, 0.0f, parent) {}
	World(
		const GLuint& shader_program,
		const float& player_x_start,
		const float& player_z_start
	) : World(shader_program, player_x_start, player_z_start, nullptr) {}
	World(
		const GLuint& shader_program,
		const float& player_x_start,
		const float& player_z_start,
		Entity* parent
	);
	~World() override;
	const Player* getPlayer();
    Text* getMenu();
	void toggleAxes();
	void setPlayerOpacity(const float& opacity);
	void movePlayer(const glm::vec3& move_vec, const float& units = 1.0f);
	bool handlePlayerKnockback();
};


#endif //PROCEDURALWORLD_WORLD_HPP

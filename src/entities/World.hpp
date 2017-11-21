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
#include "Light.h"
#include "Entity.hpp"
#include "Player.hpp"
#include "WorldOrigin.hpp"
#include "WorldTile.hpp"

class World: public Entity {
private:
	Player player;
	WorldOrigin axes;
	WorldTile* world_tile_bl;
	WorldTile* world_tile_bc;
	WorldTile* world_tile_br;
	WorldTile* world_tile_ml;
	WorldTile* world_tile_mc;
	WorldTile* world_tile_mr;
	WorldTile* world_tile_tl;
	WorldTile* world_tile_tc;
	WorldTile* world_tile_tr;
	// cell system control vectors
	//t: top m: middle b: bottom
	std::vector<int> tmb;
	//l: left c: center r: right
	std::vector<int> lcr;
	// current center tile position
	int player_current_x;
	int player_current_z;
	//int player_current_y; //for futur use
	//light
    //Light light;

public:
	explicit World(const GLuint& shader_program) : World(shader_program, nullptr) {}
	World(const GLuint& shader_program, Entity* parent);
	~World() override;
	Player* getPlayer();
	void toggleAxes();
	void extendNorth();
	void extendEast();
	void extendSouth();
	void extendWest();
	void checkPosition();
};


#endif //PROCEDURALWORLD_WORLD_HPP

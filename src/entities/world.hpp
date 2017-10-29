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

#include "entity.hpp"
#include "player.hpp"
#include "worldorigin.hpp"
#include "worldtile.hpp"

class World: public Entity {
private:
	Player player;
	WorldOrigin axes;
	WorldTile* world_tileBL;
	WorldTile* world_tileBC;
	WorldTile* world_tileBR;
	WorldTile* world_tileML;
	WorldTile* world_tileMC;
	WorldTile* world_tileMR;
	WorldTile* world_tileTL;
	WorldTile* world_tileTC;
	WorldTile* world_tileTR;
	// cell system control vectors
	std::vector<int> tmb;
	std::vector<int> lcr;
	// current center tile position
	int player_current_x;
	int player_current_z;
public:
	explicit World(const GLuint& shader_program) : World(shader_program, nullptr) {}
	World(const GLuint& shader_program, Entity* parent);
	~World() override;
	Player* getPlayer();
	void toggleAxes();
	void extendNorth();
	void extendEast();
	void checkPosition();
};


#endif //PROCEDURALWORLD_WORLD_HPP

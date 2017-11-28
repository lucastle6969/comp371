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
#include "TextB.hpp"

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
    TextB menu;
	std::vector<WorldTile*> tiles;
	// current center tile position
	int x_center;
	int z_center;
	GLuint shader_program;
	void placeWorldTile(const int& x, const int& z);
public:
	explicit World(const GLuint& shader_program) : World(shader_program, nullptr) {}
	World(const GLuint& shader_program, Entity* parent)
		: World(shader_program, 0, 0, parent) {}
	World(const GLuint& shader_program, const int& x_center, const int& z_center)
		: World(shader_program, x_center, z_center, nullptr) {}
	World(
		const GLuint& shader_program,
		const int& x_center,
		const int& z_center,
		Entity* parent
	);
	~World() override;
	Player* getPlayer();
    TextB* getMenu();
	void toggleAxes();
	void checkPosition();
};


#endif //PROCEDURALWORLD_WORLD_HPP

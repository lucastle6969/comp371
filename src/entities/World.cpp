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

#include "World.hpp"
#include "../constants.hpp"

World::World(const GLuint& shader_program, Entity* parent)
		: Entity(parent),
		  player(shader_program, this),
		  axes(shader_program, WORLD_X_MAX, WORLD_X_MAX, WORLD_Z_MAX, this),
		  tmb({1, 2, 3}),
		  lcr({1, 2, 3}),
		  player_current_x(0),
		  player_current_z(0)
{
	static const glm::vec3 initial_player_position(0.0f, 0.01f, 0.0f);

	this->player.scale(0.0005f);
	this->player.setPosition(initial_player_position);

	// hide the axes by default
	this->axes.hide();

	this->world_tile_bl = new WorldTile(shader_program, -1, 1, this);
	this->world_tile_bc = new WorldTile(shader_program, 0, 1, this);
	this->world_tile_br = new WorldTile(shader_program, 1, 1, this);
	this->world_tile_ml = new WorldTile(shader_program, -1, 0, this);
	this->world_tile_mc = new WorldTile(shader_program, 0, 0, this);
	this->world_tile_mr = new WorldTile(shader_program, 1, 0, this);
	this->world_tile_tl = new WorldTile(shader_program, -1, -1, this);
	this->world_tile_tc = new WorldTile(shader_program, 0, -1, this);
	this->world_tile_tr = new WorldTile(shader_program, 1, -1, this);
}

World::~World()
{
	delete this->world_tile_bl;
	delete this->world_tile_bc;
	delete this->world_tile_br;
	delete this->world_tile_ml;
	delete this->world_tile_mc;
	delete this->world_tile_mr;
	delete this->world_tile_tl;
	delete this->world_tile_tc;
	delete this->world_tile_tr;
}

Player* World::getPlayer()
{
	return &this->player;
}

void World::toggleAxes()
{
	this->axes.toggleHide();
}

void World::extendNorth()
{
	int rowToMoveUp = this->tmb[2];
	this->tmb[2] = this->tmb[1];
	this->tmb[1] = this->tmb[0];
	this->tmb[0] = rowToMoveUp;
	switch (rowToMoveUp) {
		case 3:
			this->world_tile_bl->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tile_bc->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tile_br->translate(glm::vec3(0.0f,0.0f,-3.0f));
			break;
		case 2:
			this->world_tile_ml->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tile_mc->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tile_mr->translate(glm::vec3(0.0f,0.0f,-3.0f));
			break;
		case 1:
			this->world_tile_tl->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tile_tc->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tile_tr->translate(glm::vec3(0.0f,0.0f,-3.0f));
			break;
	}

}

void World::extendEast()
{
	int colToMoveRight = this->lcr[0];
	this->lcr[0] = this->lcr[1];
	this->lcr[1] = this->lcr[2];
	this->lcr[2] = colToMoveRight;
	switch (colToMoveRight) {
		case 1:
			this->world_tile_tl->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tile_ml->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tile_bl->translate(glm::vec3(3.0f,0.0f,0.0f));
			break;
		case 2:
			this->world_tile_tc->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tile_mc->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tile_bc->translate(glm::vec3(3.0f,0.0f,0.0f));
			break;
		case 3:
			this->world_tile_tr->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tile_mr->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tile_br->translate(glm::vec3(3.0f,0.0f,0.0f));
			break;
	}
}

void World::extendSouth(){
	int col_to_move_down = this->tmb[0];
	this->tmb[0] = this->tmb[1];
	this->tmb[1] = this->tmb[2];
	this->tmb[2] = col_to_move_down;
	switch (col_to_move_down) {
		case 3:
			this->world_tile_bl->translate(glm::vec3(0.0f,0.0f,3.0f));
			this->world_tile_bc->translate(glm::vec3(0.0f,0.0f,3.0f));
			this->world_tile_br->translate(glm::vec3(0.0f,0.0f,3.0f));
			break;
		case 2:
			this->world_tile_ml->translate(glm::vec3(0.0f,0.0f,3.0f));
			this->world_tile_mc->translate(glm::vec3(0.0f,0.0f,3.0f));
			this->world_tile_mr->translate(glm::vec3(0.0f,0.0f,3.0f));
			break;
		case 1:
			this->world_tile_tl->translate(glm::vec3(0.0f,0.0f,3.0f));
			this->world_tile_tc->translate(glm::vec3(0.0f,0.0f,3.0f));
			this->world_tile_tr->translate(glm::vec3(0.0f,0.0f,3.0f));
			break;
	}
}

void World::extendWest(){
	int col_to_move_left = this->lcr[2];
	this->lcr[2] = this->lcr[1];
	this->lcr[1] = this->lcr[0];
	this->lcr[0] = col_to_move_left;
	switch (col_to_move_left) {
		case 1:
			this->world_tile_tl->translate(glm::vec3(-3.0f,0.0f,0.0f));
			this->world_tile_ml->translate(glm::vec3(-3.0f,0.0f,0.0f));
			this->world_tile_bl->translate(glm::vec3(-3.0f,0.0f,0.0f));
			break;
		case 2:
			this->world_tile_tc->translate(glm::vec3(-3.0f,0.0f,0.0f));
			this->world_tile_mc->translate(glm::vec3(-3.0f,0.0f,0.0f));
			this->world_tile_bc->translate(glm::vec3(-3.0f,0.0f,0.0f));
			break;
		case 3:
			this->world_tile_tr->translate(glm::vec3(-3.0f,0.0f,0.0f));
			this->world_tile_mr->translate(glm::vec3(-3.0f,0.0f,0.0f));
			this->world_tile_br->translate(glm::vec3(-3.0f,0.0f,0.0f));
			break;
	}
}

void World::checkPosition()
{
	static bool debug_position = false;

	glm::vec3 position = this->player.getPosition();
	if (debug_position) {
		std::cout << "position";
		std::cout << " x " << position.x;
		std::cout << " y " << position.y;
		std::cout << " z " << position.z;
		std::cout << std::endl;
	}

	if (floor(position.z) < this->player_current_z) {
		this->player_current_z = (int)floor(position.z);
		extendNorth();
	}
	if(floor(position.x) > this->player_current_x){
		this->player_current_x = (int)floor(position.x);
		extendEast();
	}
	if(floor(position.z) > this->player_current_z){
		this->player_current_z = (int)floor(position.z);
		extendSouth();
	}
	if(floor(position.x) < player_current_x){
		player_current_x = (int)floor(position.x);
		extendWest();
	}
}

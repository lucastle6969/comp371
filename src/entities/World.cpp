#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

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
	static const glm::vec3 initial_player_position(0.0f, 2.3f, 0.0f);

	this->player.scale(0.04f);
	this->player.setPosition(initial_player_position);

	// hide the axes by default
	this->axes.hide();

	this->world_tile_bl = new WorldTile(shader_program, glm::vec3(0.0f, 0.0f, 0.0f), this);
	this->world_tile_bc = new WorldTile(shader_program, glm::vec3(1.0f, 0.0f, 0.0f), this);
	this->world_tile_br = new WorldTile(shader_program, glm::vec3(2.0f, 0.0f, 0.0f), this);
	this->world_tile_ml = new WorldTile(shader_program, glm::vec3(0.0f, 0.0f, -1.0f), this);
	this->world_tile_mc = new WorldTile(shader_program, glm::vec3(1.0f, 0.0f, -1.0f), this);
	this->world_tile_mr = new WorldTile(shader_program, glm::vec3(2.0f, 0.0f, -1.0f), this);
	this->world_tile_tl = new WorldTile(shader_program, glm::vec3(0.0f, 0.0f, -2.0f), this);
	this->world_tile_tc = new WorldTile(shader_program, glm::vec3(1.0f, 0.0f, -2.0f), this);
	this->world_tile_tr = new WorldTile(shader_program, glm::vec3(2.0f, 0.0f, -2.0f), this);
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
	//testing move + worldTile cell system
	glm::vec3 position = this->player.getPosition();
	std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
	if ((int)position.z < this->player_current_z) {
		this->player_current_z = (int)position.z;
		extendNorth();
	}
	if((int)position.x > this->player_current_x){
		this->player_current_x = (int)position.x;
		extendEast();
	}
	if((int)position.z > this->player_current_z){
		this->player_current_z = (int)position.z;
		extendSouth();
	}
	if((int)position.x < player_current_x){
		player_current_x = (int)position.x;
		extendWest();
	}
}

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include <iostream>

#include "world.hpp"
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

	this->world_tileBL = new WorldTile(shader_program, glm::vec3(0.0f, 0.0f, 0.0f), this);
	this->world_tileBC = new WorldTile(shader_program, glm::vec3(1.0f, 0.0f, 0.0f), this);
	this->world_tileBR = new WorldTile(shader_program, glm::vec3(2.0f, 0.0f, 0.0f), this);
	this->world_tileML = new WorldTile(shader_program, glm::vec3(0.0f, 0.0f, -1.0f), this);
	this->world_tileMC = new WorldTile(shader_program, glm::vec3(1.0f, 0.0f, -1.0f), this);
	this->world_tileMR = new WorldTile(shader_program, glm::vec3(2.0f, 0.0f, -1.0f), this);
	this->world_tileTL = new WorldTile(shader_program, glm::vec3(0.0f, 0.0f, -2.0f), this);
	this->world_tileTC = new WorldTile(shader_program, glm::vec3(1.0f, 0.0f, -2.0f), this);
	this->world_tileTR = new WorldTile(shader_program, glm::vec3(2.0f, 0.0f, -2.0f), this);
}

World::~World()
{
	delete this->world_tileBL;
	delete this->world_tileBC;
	delete this->world_tileBR;
	delete this->world_tileML;
	delete this->world_tileMC;
	delete this->world_tileMR;
	delete this->world_tileTL;
	delete this->world_tileTC;
	delete this->world_tileTR;
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
			this->world_tileBL->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tileBC->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tileBR->translate(glm::vec3(0.0f,0.0f,-3.0f));
			break;
		case 2:
			this->world_tileML->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tileMC->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tileMR->translate(glm::vec3(0.0f,0.0f,-3.0f));
			break;
		case 1:
			this->world_tileTL->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tileTC->translate(glm::vec3(0.0f,0.0f,-3.0f));
			this->world_tileTR->translate(glm::vec3(0.0f,0.0f,-3.0f));
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
			this->world_tileTL->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tileML->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tileBL->translate(glm::vec3(3.0f,0.0f,0.0f));
			break;
		case 2:
			this->world_tileTC->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tileMC->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tileBC->translate(glm::vec3(3.0f,0.0f,0.0f));
			break;
		case 3:
			this->world_tileTR->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tileMR->translate(glm::vec3(3.0f,0.0f,0.0f));
			this->world_tileBR->translate(glm::vec3(3.0f,0.0f,0.0f));
			break;
	}
}

void World::checkPosition()
{
	// testing move + worldTile cell system
	glm::vec3 position = this->player.getPosition();
	std::cout<<position.x<<", "<<position.y<< ", "<<position.z<<std::endl;
	if((int)position.z<this->player_current_z){
		this->player_current_z = (int)position.z;
		this->extendNorth();
		std::cout<< "extendNorth()! "<<this->player_current_z<<std::endl;
	}

	if((int)position.x>this->player_current_x){
		this->player_current_x = (int)position.x;
		this->extendEast();
		std::cout<< "extendEast()! "<<this->player_current_x<<std::endl;
	}

}

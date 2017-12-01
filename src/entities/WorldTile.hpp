#ifndef PROCEDURALWORLD_WORLDTILE_H
#define PROCEDURALWORLD_WORLDTILE_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include <src/HitBox2d.hpp>

#include "Entity.hpp"
#include "PerlinTerrain.hpp"
#include "Rock.hpp"
#include "Trees/Tree.hpp"
#include "Rock.hpp"
#include "RockB.hpp"
#include "Text.hpp"

class WorldTile: public Entity {
private:
	static constexpr int terrain_width = 20;
	static constexpr float terrain_y_scale = 0.15f;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> elements;
	std::vector<glm::vec3> normals;
	GLuint vao;
	PerlinTerrain* terrain;
	std::vector<Rock*> rocks;
    std::vector<RockB*> rocksB;
	std::vector<Tree*> trees;
	std::vector<HitBox2d> hitboxes;
    Text seed_loc_message;
	GLuint vertices_buffer;
	GLuint element_buffer;
	GLuint normal_buffer;
public:
	WorldTile(
		const GLuint& shader_program,
		const int& world_x_location,
		const int& world_z_location,
		const float& min_hitbox_y,
		const float& max_hitbox_y,
		const HitBox2d& player_hitbox
    ) : WorldTile(
		shader_program,
		world_x_location,
		world_z_location,
		min_hitbox_y,
		max_hitbox_y,
		player_hitbox,
		nullptr
	) {}
	WorldTile(
		const GLuint& shader_program,
		const int& world_x_location,
		const int& world_z_location,
		const float& min_hitbox_y,
		const float& max_hitbox_y,
		const HitBox2d& player_hitbox,
		Entity* parent
	);
	~WorldTile() override;
	bool collidesWith(const HitBox2d& box) const;
};

#endif //PROCEDURALWORLD_WORLDTILE_H

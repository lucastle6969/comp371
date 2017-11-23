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

#include "Entity.hpp"
#include "Rock.hpp"
#include "src/entities/Trees/Tree.hpp"
#include "DrawableEntity.hpp"
#include "Rock.hpp"

class WorldTile: public DrawableEntity {
private:
	std::vector<Rock*> rocks;
	std::vector<Tree*> trees;
public:
	WorldTile(
		const GLuint& shader_program,
		const int& world_x_location,
		const int& world_z_location
    ) : WorldTile(shader_program, world_x_location, world_z_location, nullptr) {}
	WorldTile(
		const GLuint& shader_program,
		const int& world_x_location,
		const int& world_z_location,
		Entity* parent
	);
	~WorldTile() override;
	const std::vector<glm::vec3>& getVertices() override;
	GLuint getVAO() override;
	const int getColorType() override;
};

#endif //PROCEDURALWORLD_WORLDTILE_H

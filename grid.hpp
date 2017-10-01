#ifndef PACMAN3D_GRID_H
#define PACMAN3D_GRID_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include "entity.hpp"

class Grid: public Entity {
private:
	std::vector<glm::vec3> vertices;
	GLuint vao;
public:
	explicit Grid(const GLuint& shader_program) : Grid(shader_program, nullptr) {}
	Grid(const GLuint& shader_program, Entity* parent)
		: Grid(shader_program, -10, 10, -10, 10, parent) {}
	Grid(
		const GLuint& shader_program,
		const int& x_min,
		const int& x_max,
		const int& y_min,
		const int& y_max
	) : Grid(shader_program, x_min, x_max, y_min, y_max, nullptr) {}
	Grid(
		const GLuint& shader_program,
		const int& x_min,
		const int& x_max,
		const int& y_min,
		const int& y_max,
		Entity* parent
	);
	const std::vector<glm::vec3>& getVertices() override;
	GLuint getVAO() override;
	const int getColorType() override;
};

#endif //PACMAN3D_GRID_H

#ifndef PACMAN3D_GRID_H
#define PACMAN3D_GRID_H

#include "entity.hpp"

class Grid: public Entity {

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
	const int* getColorType() override;
};

#endif //PACMAN3D_GRID_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include "entity.hpp"
#include "grid.hpp"
#include "constants.hpp"

Grid::Grid(
	const GLuint& shader_program,
	const int& x_min,
	const int& x_max,
	const int& y_min,
	const int& y_max,
	Entity* parent
) : Entity(parent)
{
	this->vertices = new std::vector<glm::vec3>();
	// vertical lines
	for (int x = x_min; x <= x_max; x++) {
		this->vertices->emplace_back(x, y_min, 0.0f);
		this->vertices->emplace_back(x, x == 0 ? 0.0f : y_max, 0.0f); // leave room for y-axis
	}
	// horizontal lines
	for (int y = y_min; y <= y_max; y++) {
		this->vertices->emplace_back(x_min, y, 0.0f);
		this->vertices->emplace_back(y == 0 ? 0.0f : x_max, y, 0.0f); // leave room for x-axis
	}

	this->initVertexArray(shader_program);
}

const int* Grid::getColorType()
{
	return &COLOR_WHITE;
}

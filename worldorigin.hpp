#ifndef PACMAN3D_WORLDORIGIN_H
#define PACMAN3D_WORLDORIGIN_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include "entity.hpp"

class WorldOrigin: public Entity {

public:
	explicit WorldOrigin(const GLuint& shader_program) : WorldOrigin(shader_program, nullptr) {}
	WorldOrigin(const GLuint& shader_program, Entity* parent)
		: WorldOrigin(shader_program, 10, 10, 10, parent) {}
	WorldOrigin(
		const GLuint& shader_program,
		const int& x_max,
		const int& y_max,
		const int& z_max,
		Entity* parent
	);
	const int* getColorType() override;
	const GLenum* getDrawMode() override;
};

#endif //PACMAN3D_WORLDORIGIN_H

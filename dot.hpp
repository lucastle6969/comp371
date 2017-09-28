#ifndef PACMAN3D_DOT_H
#define PACMAN3D_DOT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include "entity.hpp"

class Dot: public Entity {

public:
	explicit Dot(const GLuint& shader_program) : Dot(shader_program, nullptr) {}
	Dot(const GLuint& shader_program, Entity* parent);
	const int* getColorType() override;
};

#endif //PACMAN3D_DOT_H

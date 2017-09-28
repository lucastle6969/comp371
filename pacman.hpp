#ifndef PACMAN3D_PACMAN_H
#define PACMAN3D_PACMAN_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include "entity.hpp"

class Pacman: public Entity {

public:
	explicit Pacman(const GLuint& shader_program) : Pacman(shader_program, nullptr) {}
	Pacman(const GLuint& shader_program, Entity* parent);
	const int* getColorType() override;
};

#endif //PACMAN3D_PACMAN_H

#ifndef PACMAN3D_PACMAN_H
#define PACMAN3D_PACMAN_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include "entity.hpp"

class Pacman: public Entity {
private:
	static constexpr float teapot_rotation_angle = (float)(M_PI / 2);
	static constexpr float teapot_scale_ratio = 13.0f;
	const glm::mat4& getBaseRotation() override;
	const glm::vec3& getDefaultFaceVector() override;
	std::vector<glm::vec3> pacman_vertices;
	std::vector<glm::vec3> teapot_vertices;
	GLuint pacman_vao;
	GLuint teapot_vao;
	bool using_teapot;
public:
	explicit Pacman(const GLuint& shader_program) : Pacman(shader_program, nullptr) {}
	Pacman(const GLuint& shader_program, Entity* parent);
	const std::vector<glm::vec3>& getVertices() override;
	GLuint getVAO() override;
	const int getColorType() override;
	bool toggleTeapot();
};

#endif //PACMAN3D_PACMAN_H

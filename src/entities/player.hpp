#ifndef COMP371_PLAYER_HPP
#define COMP371_PLAYER_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include "entity.hpp"
#include "drawableentity.hpp"

class Player: public DrawableEntity {
private:
	static constexpr float base_rotation_angle = (float)(M_PI / 2);
	const glm::mat4& getBaseRotation() override;
	const glm::vec3& getDefaultFaceVector() override;
	std::vector<glm::vec3> vertices;
	GLuint vao;
public:
	explicit Player(const GLuint& shader_program) : Player(shader_program, nullptr) {}
	Player(const GLuint& shader_program, Entity* parent);
	const std::vector<glm::vec3>& getVertices() override;
	GLuint getVAO() override;
	const int getColorType() override;
};

#endif // COMP371_PLAYER_HPP

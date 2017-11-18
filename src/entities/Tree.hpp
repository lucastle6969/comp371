#ifndef PROCEDURALWORLD_TREE_HPP
#define PROCEDURALWORLD_TREE_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include "Entity.hpp"
#include "DrawableEntity.hpp"

class Tree : public DrawableEntity {
private:
	std::vector<glm::vec3> vertices;
	GLuint vao;
public:
	Tree(
		const GLuint& shader_program,
		const float& world_x_location,
		const float& world_z_location,
		const float& base_span
	) : Tree(
			shader_program,
			world_x_location,
			world_z_location,
			base_span,
			nullptr
	) {}
	Tree(
		const GLuint& shader_program,
		const float& world_x_location,
		const float& world_z_location,
		const float& base_span,
		Entity* parent
	);
	const std::vector<glm::vec3>& getVertices() override;
	GLuint getVAO() override;
	const int getColorType() override;
};


#endif //PROCEDURALWORLD_TREE_HPP

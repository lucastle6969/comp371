#ifndef PROCEDURALWORLD_PERLINTERRAIN_HPP
#define PROCEDURALWORLD_PERLINTERRAIN_HPP

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

class PerlinTerrain : public DrawableEntity {
private:
	float x_span;
	float z_span;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> elements;
	std::vector<glm::vec3> normals;
	GLuint vao;
	GLuint vertices_buffer;
	GLuint element_buffer;
	GLuint normal_buffer;
	const glm::vec3& getPoint(const float& x_i, const float& z_i) const;
public:
	PerlinTerrain(
			const GLuint& shader_program,
			const int& world_x_location,
			const int& world_z_location,
			const int& x_span,
			const int& z_span
	) : PerlinTerrain(
			shader_program,
			world_x_location,
			world_z_location,
			x_span,
			z_span,
			nullptr
	) {}
	PerlinTerrain(
			const GLuint& shader_program,
			const int& world_x_location,
			const int& world_z_location,
			const int& x_span,
			const int& z_span,
			Entity* parent
	);
	~PerlinTerrain() override;
	const std::vector<glm::vec3>& getVertices() const override;
	GLuint getVAO() override;
	const int getColorType() override;
	glm::vec3 findIntersectionPoint(const float& x, const float& z) const;
};


#endif //PROCEDURALWORLD_PERLINTERRAIN_HPP

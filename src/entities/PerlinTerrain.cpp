#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include <src/utils.hpp>
#include <src/constants.hpp>

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "PerlinTerrain.hpp"

PerlinTerrain::PerlinTerrain(
	const GLuint &shader_program,
	const int& world_x_location,
	const int& world_z_location,
	const int& x_span,
	const int& z_span,
	Entity *parent
) : DrawableEntity(shader_program, parent)
{
	this->draw_mode = GL_TRIANGLES;

	this->setMaterial(
			glm::vec3(.5,.5,.5), // need to change this to some other value... maybe the height of the plane if we ever make it.
			glm::vec3(.5,.5,.5),
			glm::vec3(.25,.25,.25),
			25.0f
	);

	// generate terrain vertices via Improved Perlin Noise
	for (int z_i = 0; z_i < z_span; z_i++) {
		for (int x_i = 0; x_i < x_span; x_i++) {
			auto x = (float)x_i / (x_span - 1);
			auto z = (float)z_i / (z_span - 1);
			this->vertices.emplace_back(
					x,
					utils::getPerlinNoise(world_x_location + x, world_z_location + z),
					z
			);
		}
	}

	// generate terrain elements
	for (int z_i = z_span - 1; z_i--; ) {
		int offset = z_i * x_span;
		for (int x_i = x_span - 1; x_i--; ) {
			// first triangle
			this->elements.emplace_back(offset + x_i); // top-left
			this->elements.emplace_back(offset + x_span + x_i); // bottom-left
			this->elements.emplace_back(offset + x_i + 1); // top-right

			// second triangle
			this->elements.emplace_back(offset + x_span + x_i); // bottom-left
			this->elements.emplace_back(offset + x_span + x_i + 1); // bottom-right
			this->elements.emplace_back(offset + x_i + 1); // top-right
		}
	}

	// generate terrain normals
	float curr_x, curr_z;
	float before_x, after_x, before_z, after_z;
	glm::vec3 point_before_x, point_after_x, point_before_z, point_after_z;
	for (size_t i = 0, len = this->vertices.size(); i < len; i++) {
		// compute normals using vertices within tile when applicable, or else
		// compute nearby vertices in adjacent tiles

		// find point before along x axis
		if (i % x_span > 0) {
			point_before_x = this->vertices[i - 1];
		} else {
			before_x = world_x_location - 1.0f / (x_span - 1);
			curr_z = this->vertices[i].z;
			point_before_x = glm::vec3(
					before_x,
					utils::getPerlinNoise(before_x, curr_z),
					curr_z
			);
		}
		// find point after along x axis
		if (i % (x_span + 1) > 0) {
			point_after_x = this->vertices[i + 1];
		} else {
			after_x = world_x_location + 1.0f / (x_span - 1);
			curr_z = this->vertices[i].z;
			point_after_x = glm::vec3(
					after_x,
					utils::getPerlinNoise(after_x, curr_z),
					curr_z
			);
		}
		// find point before along z axis
		if (i >= x_span) {
			point_before_z = this->vertices[i - x_span];
		} else {
			curr_x = this->vertices[i].x;
			before_z = world_z_location - 1.0f / (z_span - 1);
			point_before_z = glm::vec3(
					curr_x,
					utils::getPerlinNoise(curr_x, before_z),
					before_z
			);
		}
		// find point after along z axis
		if (i + x_span < len) {
			point_after_z = this->vertices[i + x_span];
		} else {
			curr_x = this->vertices[i].x;
			after_z = world_z_location + 1.0f / (z_span - 1);
			point_before_z = glm::vec3(
					curr_x,
					utils::getPerlinNoise(curr_x, after_z),
					after_z
			);
		}

		// cross product of vector between adjacent points along z axis and vector
		// between adjacent points along x axis forms approximate normal vector
		this->normals.emplace_back(glm::normalize(glm::cross(
				point_after_z - point_before_z,
				point_after_x - point_before_x
		)));
	}

	// initialize terrain vao
	this->vao = this->initVertexArray(
			this->vertices,
			this->elements,
			this->normals,
			&this->vertices_buffer,
			&this->element_buffer,
			&this->normal_buffer
	);
}

PerlinTerrain::~PerlinTerrain()
{
	glDeleteBuffers(1, &this->vertices_buffer);
	glDeleteBuffers(1, &this->element_buffer);
	glDeleteBuffers(1, &this->normal_buffer);
	glDeleteVertexArrays(1, &this->vao);
}

const std::vector<glm::vec3>& PerlinTerrain::getVertices() const
{
	return this->vertices;
}

GLuint PerlinTerrain::getVAO() {
	return this->vao;
}

const int PerlinTerrain::getColorType() {
	return COLOR_HEIGHT;
}

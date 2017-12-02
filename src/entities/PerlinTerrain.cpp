#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>
#include <cmath>

#include <src/utils.hpp>
#include <src/constants.hpp>

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "PerlinTerrain.hpp"
#include <iostream>
PerlinTerrain::PerlinTerrain(
	const GLuint &shader_program,
	const int& world_x_location,
	const int& world_z_location,
	const int& x_span,
	const int& z_span,
	Entity *parent
) : DrawableEntity(shader_program, parent),
    x_span(x_span),
    z_span(z_span)
{
	this->draw_mode = GL_TRIANGLES;

	this->setMaterial(
			glm::vec3(0.05f, 0.1f, 0.0f),
			glm::vec3(0.25f, 0.5f, 0.0f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			1.0f
	);

	// generate terrain vertices via Improved Perlin Noise
	for (int z_i = 0; z_i < z_span; z_i++) {
		auto z = (float)z_i / (z_span - 1);
		for (int x_i = 0; x_i < x_span; x_i++) {
			auto x = (float)x_i / (x_span - 1);
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
			before_x = 0 - 1.0f / (x_span - 1);
			curr_z = this->vertices[i].z;
			point_before_x = glm::vec3(
					before_x,
					utils::getPerlinNoise(
							world_x_location + before_x,
							world_z_location + curr_z
					),
					curr_z
			);
		}
		// find point after along x axis
		if ((i + 1) % x_span > 0) {
			point_after_x = this->vertices[i + 1];
		} else {
			after_x = 1 + 1.0f / (x_span - 1);
			curr_z = this->vertices[i].z;
			point_after_x = glm::vec3(
					after_x,
					utils::getPerlinNoise(
							world_x_location + after_x,
							world_z_location + curr_z
					),
					curr_z
			);
		}
		// find point before along z axis
		if (i >= x_span) {
			point_before_z = this->vertices[i - x_span];
		} else {
			curr_x = this->vertices[i].x;
			before_z = 0 - 1.0f / (z_span - 1);
			point_before_z = glm::vec3(
					curr_x,
					utils::getPerlinNoise(
							world_x_location + curr_x,
							world_z_location + before_z
					),
					before_z
			);
		}
		// find point after along z axis
		if (i + x_span < len) {
			point_after_z = this->vertices[i + x_span];
		} else {
			curr_x = this->vertices[i].x;
			after_z = 1 + 1.0f / (z_span - 1);
			point_after_z = glm::vec3(
					curr_x,
					utils::getPerlinNoise(
							world_x_location + curr_x,
							world_z_location + after_z
					),
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
	return COLOR_LIGHTING;
}

glm::vec3 PerlinTerrain::findIntersectionPoint(const float& x, const float& z) const
{
	// down vector intersecting triangle
	static glm::vec3 down(0.0f, -1.0f, 0.0f);

	// compute the lower coordinate of the piece of terrain we're on
	auto x_i = (int)floor(x * (this->x_span - 1));
	auto z_i = (int)floor(z * (this->z_span - 1));

	glm::vec3 normal;
	// find the triangle we're located above and compute its normal
	const glm::vec3& bottom_left = this->getPoint(x_i, z_i);
	const glm::vec3& bottom_right = this->getPoint(x_i + 1, z_i);
	const glm::vec3& top_left = this->getPoint(x_i, z_i + 1);
	const glm::vec3& top_right = this->getPoint(x_i + 1, z_i + 1);
	glm::vec2 bl2d(bottom_left.x, bottom_left.z);
	glm::vec2 tr2d(top_right.x, top_right.z);
	glm::vec2 p2d(x, z);
	if (glm::length(p2d - bl2d) < glm::length(p2d - tr2d)) {
		normal = glm::normalize(glm::cross(
				bottom_right - bottom_left,
				top_left - bottom_left
		));
	} else {
		normal = glm::normalize(glm::cross(
				top_left - top_right,
				bottom_right - top_right
		));
	}

	// arbitrary ray origin safely above terrain
	glm::vec3 ray_origin(x, 1.0f, z);

	// ray-plane intersection: https://stackoverflow.com/a/23976134/4956731
	float denom = glm::dot(normal, down);
	float t = glm::dot(bottom_left - ray_origin, normal) / denom;
	static bool print_info = true;
	if (print_info) {
		std::cout << std::endl;
		std::cout << "x: " << x << ", x_i: " << x_i << ", z: " << z << ", z_i: " << z_i
		          << std::endl;
		std::cout << "bL: " << bottom_left.x << ", " << bottom_left.y << ", "
		          << bottom_left.z << std::endl;
		std::cout << "bR: " << bottom_right.x << ", " << bottom_right.y << ", "
		          << bottom_right.z << std::endl;
		std::cout << "tL: " << top_left.x << ", " << top_left.y << ", " << top_left.z
		          << std::endl;
		std::cout << "tR: " << top_right.x << ", " << top_right.y << ", " << top_right.z
		          << std::endl;
		std::cout << "normal: " << normal.x << ", " << normal.y << ", " << normal.z
		          << std::endl;
		std::cout << "denom: " << denom << std::endl;
		std::cout << "t: " << t << std::endl;
	}
	return ray_origin + t * down;
}

const glm::vec3& PerlinTerrain::getPoint(const float& x_i, const float& z_i) const
{
	return this->vertices[z_i * this->x_span + x_i];
}

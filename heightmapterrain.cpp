#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

#include "entity.hpp"
#include "constants.hpp"
#include "heightmapterrain.hpp"

HeightMapTerrain::HeightMapTerrain(
	const GLuint &shader_program,
	const std::string& map_path,
	Entity* parent
) : Entity(parent)
{
	static const int requested_channels = 3;

	int map_width, map_height, channels;
	// Read image data using 3 channels (red, green, blue)
	float* image_data = stbi_loadf(map_path.c_str(), &map_width, &map_height, &channels, requested_channels);

	if (!image_data) {
		throw std::runtime_error("Failed to load image " + map_path + ": " + stbi_failure_reason());
	}

	// BEGIN STEP 1

	for (int i = 0, limit = map_width * map_height * requested_channels; i < limit; i += requested_channels) {
		// average of color values becomes y value (height); values are between 0 and 1.
		// averaging compensates for non-greyscale images
		float height = 0;
		for (int j = 0; j < requested_channels; j++) {
			height += image_data[i + j];
		}
		height /= requested_channels;

		// use x and y values of 2d image to get x and z in 3d space, centered around origin
		int pixel_number = i / requested_channels;
		float x = (pixel_number % map_width) - (map_width / 2.0f);
		float z = (pixel_number / map_width) - (map_height / 2.0f);

		this->vertices.emplace_back(x, height, z);
	}

	// Free image data memory since we're not using it anymore
	stbi_image_free(image_data);

	std::vector<GLuint> elements;
	HeightMapTerrain::createElements(map_width, map_height, &elements);
	this->vao = Entity::initVertexArray(shader_program, this->vertices, elements);

	// END STEP 1

	// BEGIN STEP 2

	// TODO: get skip size as input from user
	int skip_size = 20;

	std::vector<glm::vec3> reduced_vertices;
	for (int y = 0; y < map_height; y += skip_size) {
		int offset = y * map_width;
		for (int x = 0; x < map_width; x += skip_size) {
			reduced_vertices.push_back(this->vertices[offset + x]);

			int x_remaining = map_width - x;
			if (x_remaining > 1 && x_remaining <= skip_size) {
				reduced_vertices.push_back(this->vertices[offset + map_width - 1]);
			}
		}

		int y_remaining = map_height - y;
		if (y_remaining > 1 && y_remaining <= skip_size) {
			offset = (map_height - 1) * map_width;
			for (int x = 0; x < map_width; x += skip_size) {
				reduced_vertices.push_back(this->vertices[offset + x]);

				int x_remaining = map_width - x;
				if (x_remaining > 1 && x_remaining <= skip_size) {
					reduced_vertices.push_back(this->vertices[offset + map_width - 1]);
				}
			}
		}
	}
	this->vertices = reduced_vertices;

	// sampled vertices should contain first and last vertex in each row/column,
	// regardless of skip interval
	int reduced_width = map_width / skip_size + (map_width % skip_size == 0 ? 1 : 2);
	int reduced_height = map_height / skip_size + (map_height % skip_size == 0 ? 1 : 2);

	std::vector<GLuint> elements2;
	HeightMapTerrain::createElements(reduced_width, reduced_height, &elements2);
	this->vao = Entity::initVertexArray(shader_program, reduced_vertices, elements2);

	// END STEP 2

	// since the model y coordinates are in a range between 0 and 1,
	// we should scale them to look more pronounced.
	// arbitrarily choosing average of width and height divided by 4
	float y_scale = (map_width + map_height) / 8.0f;
	this->base_scale = glm::scale(this->base_scale, glm::vec3(1.0f, y_scale, 1.0f));

	this->draw_mode = GL_POINTS;
}

const std::vector<glm::vec3>& HeightMapTerrain::getVertices()
{
	return this->vertices;
}

GLuint HeightMapTerrain::getVAO()
{
	return this->vao;
}

const int HeightMapTerrain::getColorType()
{
	return COLOR_HEIGHT;
}

const glm::mat4& HeightMapTerrain::getBaseScale()
{
	return this->base_scale;
}

const glm::mat4& HeightMapTerrain::getBaseRotation()
{
	static glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	static glm::mat4 identity;
	static glm::mat4 rotation = glm::rotate(identity, HeightMapTerrain::rotation_angle, x_axis);

	return rotation;
}

void HeightMapTerrain::createElements(
	const int& width,
	const int& height,
	std::vector<GLuint>* const elements
) {
	if (elements == nullptr) {
		throw std::runtime_error("Elements vector must not be null.");
	}

	// since we know our terrain comes from an image, i.e. a fully-filled 2d array,
	// we can create an element array using an uncomplicated algorithm which creates
	// two triangles to connect each arrangement of 4 adjacent vertices.
	for (int y = height - 1; y--; ) {
		int offset = y * width;
		for (int x = width - 1; x--; ) {
			// first triangle
			elements->emplace_back(offset + x); // top-left
			elements->emplace_back(offset + x + 1); // top-right
			elements->emplace_back(offset + width + x); // bottom-left

			// second triangle
			elements->emplace_back(offset + width + x); // bottom-left
			elements->emplace_back(offset + x + 1); // top-right
			elements->emplace_back(offset + width + x + 1); // bottom-right
		}
	}
}

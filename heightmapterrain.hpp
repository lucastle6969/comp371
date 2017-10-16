#ifndef HEIGHTMAPPER_HEIGHTMAPTERRAIN_HPP
#define HEIGHTMAPPER_HEIGHTMAPTERRAIN_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "entity.hpp"

class HeightMapTerrain: public Entity {
private:
	static constexpr float rotation_angle = (float)(M_PI / 2);
	// STEP 1: Points taken directly from pixel color values
	std::vector<glm::vec3> vertices_step_1;
	GLuint vao_step_1;
	// STEP 2: Points reduced via skip interval
	std::vector<glm::vec3> vertices_step_2;
	GLuint vao_step_2;
	// STEP 3: Missing points from step 2 spline-interpolated along x-axis
	std::vector<glm::vec3> vertices_step_3;
	GLuint vao_step_3;
	// STEP 4: Remaining missing points from step 3 spline-interpolated along z-axis
	std::vector<glm::vec3> vertices_step_4;
	GLuint vao_step_4;
	glm::mat4 base_scale;
	static void createElements(const int& width, const int& height, std::vector<GLuint>* const elements);
	const glm::mat4& getBaseScale() override;
	const glm::mat4& getBaseRotation() override;
public:
	HeightMapTerrain(const GLuint& shader_program, const std::string& map_path)
		: HeightMapTerrain(shader_program, map_path, nullptr) {}
	HeightMapTerrain(const GLuint& shader_program, const std::string& map_path, Entity* parent);
	const std::vector<glm::vec3>& getVertices() override;
	GLuint getVAO() override;
	const int getColorType() override;
};

#endif //HEIGHTMAPPER_HEIGHTMAPTERRAIN_HPP

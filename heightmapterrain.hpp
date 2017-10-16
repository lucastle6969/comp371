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
	std::vector<glm::vec3> vertices;
	GLuint vao;
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

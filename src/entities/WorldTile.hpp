#ifndef PROCEDURALWORLD_WORLDTILE_H
#define PROCEDURALWORLD_WORLDTILE_H

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

class WorldTile: public DrawableEntity {
private:
    static constexpr float base_rotation_angle = (float)(0);
    const glm::mat4& getBaseRotation() override;
    const glm::vec3& getDefaultFaceVector() override;

public:
    WorldTile(const GLuint& shader_program, const int& x_location, const int& z_location)
		: WorldTile(shader_program, x_location, z_location, nullptr) {}
    WorldTile(
		const GLuint& shader_program,
		const int& x_location,
		const int& z_location,
		Entity* parent
    );
    const std::vector<glm::vec3>& getVertices() override;
    GLuint getVAO() override;
    const int getColorType() override;
};

#endif //PROCEDURALWORLD_WORLDTILE_H

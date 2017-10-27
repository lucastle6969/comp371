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

#include "entity.hpp"
#include "drawableentity.hpp"

class WorldTile: public DrawableEntity {
private:
    static constexpr float base_rotation_angle = 0;
    const glm::mat4& getBaseTranslation() override;
    const glm::vec3& getDefaultFaceVector() override;
    std::vector<glm::vec3> vertices;
    glm::vec3 initialTranslation;
    GLuint vao;
    GLuint vertices_buffer;
    GLuint element_buffer;
    static void createElements(const int& width, const int& height, std::vector<GLuint>* const elements);

public:
    explicit WorldTile(const GLuint& shader_program) : WorldTile(shader_program, nullptr) {}
    WorldTile(const GLuint& shader_program, Entity* parent);
    WorldTile(const GLuint& shader_program, Entity* parent, glm::vec3 initialTranslation);
    const std::vector<glm::vec3>& getVertices() override;
    GLuint getVAO() override;
    const int getColorType() override;
};

#endif //PROCEDURALWORLD_WORLDTILE_H






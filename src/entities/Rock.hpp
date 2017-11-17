//
// Created by Carlo Gentile on 2017-11-09.
//

#ifndef PROCEDURALWORLD_ROCK_HPP
#define PROCEDURALWORLD_ROCK_HPP

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


class Rock: public DrawableEntity {
private:
    std::vector<glm::vec3> vertices;
    GLuint vao;
    GLuint vertices_buffer;
    GLuint element_buffer;
    GLuint uv_buffer;

public:
    Rock(
        const GLuint &shader_program,
        const float& x_location,
        const float& z_location,
        const float& x_span,
        const float& z_span
    ) : Rock(shader_program, x_location, z_location, x_span, z_span, nullptr) {}
    Rock(
        const GLuint &shader_program,
        const float& x_location,
        const float& z_location,
        const float& x_span,
        const float& z_span,
        Entity *parent
    );
    const std::vector<glm::vec3>& getVertices() override;
    GLuint getVAO() override;
    const int getColorType() override;
};


#endif //PROCEDURALWORLD_ROCK_HPP

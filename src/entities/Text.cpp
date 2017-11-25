#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Text.hpp"

#include "../constants.hpp"
#include "../loadTexture.hpp"

Text::Text(
const GLuint& shader_program,
const GLuint& message,
        Entity* parent
) : DrawableEntity(shader_program, parent)
{
    static const std::vector<glm::vec3> vertices = {
            glm::vec3(0.0f,0.0f,0.0f),
            glm::vec3(2.0f,0.0f,0.0f),
            glm::vec3(0.0f,2.0f,0.0f),
            glm::vec3(2.0f,2.0f,0.0f)
    };

    static const std::vector<glm::vec3> normals = {
            glm::vec3(0.0f,0.0f,1.0f),
            glm::vec3(0.0f,0.0f,1.0f),
            glm::vec3(0.0f,0.0f,1.0f),
            glm::vec3(0.0f,0.0f,1.0f)
    };


    static const std::vector<GLuint> elements = {
            2, 0, 1, 2, 1, 3
    };

    static const std::vector<glm::vec2> uvs = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 1.0f)

    };

    this->draw_mode = GL_TRIANGLES;

    this->vao = DrawableEntity::initVertexArray(
            vertices,
            elements,
            normals,
            uvs,
            &this->vertices_buffer,
            &this->element_buffer,
            &this->normal_buffer,
            &this->uv_buffer
    );

    const std::vector<glm::vec3>& Text::getVertices()
    {
        return vertices;
    }

    GLuint Text::getVAO()
    {
        return this->vao;
    }

    const int Text::getColorType()
    {
        return COLOR_TEXTURE;
    }

    GLuint Text::getTextureId()
    {
        GLuint text_texture = loadTexture(
                "../textures/welcomeTXT.png",
                GL_LINEAR,
                GL_LINEAR,
                true
        );
        return text_texture;
    }

}


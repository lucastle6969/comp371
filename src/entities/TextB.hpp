#ifndef PROCEDURALWORLD_TEXTB_HPP
#define PROCEDURALWORLD_TEXTB_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <string>
#include "Entity.hpp"
#include "DrawableEntity.hpp"

class TextB : public DrawableEntity {
private:
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> elements;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2>uvs;
    GLuint vao;
    GLuint vertices_buffer;
    GLuint element_buffer;
    GLuint uv_buffer;
    GLuint normal_buffer;
public:
    TextB(
            const GLuint& shader_program,
            const std::string& message,
            const int line_height,
            const float left_margin,
            const unsigned int FONT_STYLE
    ) : TextB(
            shader_program,
            message,
            line_height,
            left_margin,
            FONT_STYLE,
            nullptr
    ){}
    TextB(
            const GLuint& shader_program,
            const std::string& message,
            const int line_height,
            const float left_margin,
            const unsigned int FONT_STYLE,
            Entity* parent
    );
    ~TextB() override;
    const std::vector<glm::vec3>& getVertices() override;
    GLuint getVAO() override;
    const int getColorType() override;
    GLuint getTextureId() override;

};



#endif //PROCEDURALWORLD_TEXTB_HPP

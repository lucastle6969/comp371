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

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "TextB.hpp"

#include "../constants.hpp"
#include "../loadTexture.hpp"

TextB::TextB(
        const GLuint& shader_program,
        const std::string& message,
        const int line_height,
        const int left_margin,
        Entity* parent
) : DrawableEntity(shader_program, parent) {

    this->draw_mode = GL_TRIANGLES;

    int row_from_bottom;
    if(line_height>99){
        row_from_bottom = 99;
    }else{
        row_from_bottom = line_height;
    }

    float char_width = 0.01;

    for(int i=0; i<message.length(); i++){
        if (i>100)
            break;
        this->vertices.emplace_back(i * char_width, line_height * char_width, 0.0f);
        this->vertices.emplace_back(i * char_width + char_width, line_height * char_width, 0.0f);
        this->vertices.emplace_back(i * char_width, line_height * char_width + char_width, 0.0f);
        this->vertices.emplace_back(i * char_width + char_width, line_height * char_width + char_width, 0.0f);

        this->normals.emplace_back(1.0f, 1.0f, 1.0f);
        this->normals.emplace_back(1.0f, 1.0f, 1.0f);
        this->normals.emplace_back(1.0f, 1.0f, 1.0f);
        this->normals.emplace_back(1.0f, 1.0f, 1.0f);

        this->elements.emplace_back(i+2);
        this->elements.emplace_back(i);
        this->elements.emplace_back(i+1);
        this->elements.emplace_back(i+2);
        this->elements.emplace_back(i+1);
        this->elements.emplace_back(i+3);

        switch(message[i]){
            case 'a':
                this->uvs.emplace_back(0, 0.833333);
                this->uvs.emplace_back(0.166666, 0.833333);
                this->uvs.emplace_back(0, 1);
                this->uvs.emplace_back(0.166666, 1);
                break;
            case 'b':
                this->uvs.emplace_back(0.166666, 0.833333);
                this->uvs.emplace_back(0.333333, 0.833333);
                this->uvs.emplace_back(0.166666, 1);
                this->uvs.emplace_back(0.333333, 1);
                break;
            case 'c':
                this->uvs.emplace_back(0.333333, 0.833333);
                this->uvs.emplace_back(0.5, 0.833333);
                this->uvs.emplace_back(0.333333, 1);
                this->uvs.emplace_back(0.5, 1);
                break;
            case 'd':
                this->uvs.emplace_back(0.5, 0.833333);
                this->uvs.emplace_back(0.666666, 0.833333);
                this->uvs.emplace_back(0.5, 1);
                this->uvs.emplace_back(0.666666, 1);
                break;
            case 'e':
                this->uvs.emplace_back(0.666666, 0.833333);
                this->uvs.emplace_back(0.833333, 0.833333);
                this->uvs.emplace_back(0.666666, 1);
                this->uvs.emplace_back(0.833333, 1);
                break;
            case 'f':
                this->uvs.emplace_back(0.833333, 0.833333);
                this->uvs.emplace_back(1, 0.833333);
                this->uvs.emplace_back(0.833333, 1);
                this->uvs.emplace_back(1, 1);
                break;
            case 'g':
                this->uvs.emplace_back(0, 0.666666);
                this->uvs.emplace_back(0.166666, 0.666666);
                this->uvs.emplace_back(0, 0.833333);
                this->uvs.emplace_back(0.166666, 0.833333);
                break;
            case 'h':
                this->uvs.emplace_back(0.166666, 0.666666);
                this->uvs.emplace_back(0.333333, 0.666666);
                this->uvs.emplace_back(0.166666, 0.833333);
                this->uvs.emplace_back(0.333333, 0.833333);
                break;
            case 'i':
                this->uvs.emplace_back(0.333333, 0.666666);
                this->uvs.emplace_back(0.5, 0.666666);
                this->uvs.emplace_back(0.333333, 0.833333);
                this->uvs.emplace_back(0.5, 0.833333);
                break;
            case 'j':
                this->uvs.emplace_back(0.5, 0.666666);
                this->uvs.emplace_back(0.666666, 0.666666);
                this->uvs.emplace_back(0.5, 0.833333);
                this->uvs.emplace_back(0.666666, 0.833333);
                break;
            case 'k':
                this->uvs.emplace_back(0.666666, 0.666666);
                this->uvs.emplace_back(0.833333, 0.666666);
                this->uvs.emplace_back(0.666666, 0.833333);
                this->uvs.emplace_back(0.833333, 0.833333);
                break;
            case 'l':
                this->uvs.emplace_back(0.833333, 0.666666);
                this->uvs.emplace_back(1, 0.666666);
                this->uvs.emplace_back(0.833333, 0.833333);
                this->uvs.emplace_back(1, 0.833333);
                break;
            case 'm':
                break;
            case 'n':
                break;
            case 'o':
                break;
            case 'p':
                break;
            case 'q':
                break;
            case 'r':
                break;
            case 's':
                break;
            case 't':
                break;
            case 'u':
                break;
            case 'v':
                break;
            case 'w':
                break;
            case 'x':
                break;
            case 'y':
                break;
            case 'z':
                break;
            case '0':
                break;
            case '1':
                break;
            case '2':
                break;
            case '3':
                break;
            case '4':
                break;
            case '5':
                break;
            case '6':
                break;
            case '7':
                break;
            case '8':
                break;
            case '9':
                break;
            case ' ':
                break;
            case ',':
                break;
            case '.':
                break;

        }
    }

    this->vao = DrawableEntity::initVertexArray(
            this->vertices,
            this->elements,
            this->normals,
            this->uvs,
            &this->vertices_buffer,
            &this->element_buffer,
            &this->normal_buffer,
            &this->uv_buffer
    );


}

TextB::~TextB()
{
    glDeleteBuffers(1, &this->vertices_buffer);
    glDeleteBuffers(1, &this->element_buffer);
    glDeleteBuffers(1, &this->uv_buffer);
    glDeleteBuffers(1, &this->normal_buffer);
    glDeleteVertexArrays(1, &this->vao);
}

    const std::vector<glm::vec3>& TextB::getVertices()
    {
        return this->vertices;
    }

    GLuint TextB::getVAO()
    {
        return this->vao;
    }

    const int TextB::getColorType()
    {
        return COLOR_FONT;
    }

    GLuint TextB::getTextureId()
    {
        GLuint text_texture = loadTexture(
                "../textures/mythos_text_map_medium.png",
                GL_LINEAR,
                GL_LINEAR,
                true
        );
        return text_texture;
    }





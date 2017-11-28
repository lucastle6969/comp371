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

//        this->vertices.emplace_back(0,0,0);
//        this->vertices.emplace_back(1,0,0);
//        this->vertices.emplace_back(0,1,0);
//        this->vertices.emplace_back(1,1,0);

        this->normals.emplace_back(0.0f, 0.0f, 1.0f);
        this->normals.emplace_back(0.0f, 0.0f, 1.0f);
        this->normals.emplace_back(0.0f, 0.0f, 1.0f);
        this->normals.emplace_back(0.0f, 0.0f, 1.0f);

        this->elements.emplace_back(i+2);
        this->elements.emplace_back(i);
        this->elements.emplace_back(i+1);
        this->elements.emplace_back(i+2);
        this->elements.emplace_back(i+1);
        this->elements.emplace_back(i+3);


        const float x_width = 6.0f;
        const float y_width = 6.0f;
        int x_offset;
        int y_offset;
        switch(message[i]){
            case 'a':
            case 'A':
                x_offset = 0;
                y_offset = 5;
                break;
            case 'b':
            case 'B':
                x_offset = 1;
                y_offset = 5;
                break;
            case 'c':
            case 'C':
                x_offset = 2;
                y_offset = 5;
                break;
            case 'd':
            case 'D':
                x_offset = 3;
                y_offset = 5;
                break;
            case 'e':
            case 'E':
                x_offset = 4;
                y_offset = 5;
                break;
            case 'f':
            case 'F':
                x_offset = 5;
                y_offset = 5;
                break;
            case 'g':
            case 'G':
                x_offset = 0;
                y_offset = 4;
                break;
            case 'h':
            case 'H':
                x_offset = 1;
                y_offset = 4;
                break;
            case 'i':
            case 'I':
                x_offset = 2;
                y_offset = 4;
                break;
            case 'j':
            case 'J':
                x_offset = 3;
                y_offset = 4;
                break;
            case 'k':
            case 'K':
                x_offset = 4;
                y_offset = 4;
                break;
            case 'l':
            case 'L':
                x_offset = 5;
                y_offset = 4;
                break;
            case 'm':
            case 'M':
                x_offset = 0;
                y_offset = 3;
                break;
            case 'n':
            case 'N':
                x_offset = 1;
                y_offset = 3;
                break;
            case 'o':
            case 'O':
                x_offset = 2;
                y_offset = 3;
                break;
            case 'p':
            case 'P':
                x_offset = 3;
                y_offset = 3;
                break;
            case 'q':
            case 'Q':
                x_offset = 4;
                y_offset = 3;
                break;
            case 'r':
            case 'R':
                x_offset = 5;
                y_offset = 3;
                break;
            case 's':
            case 'S':
                x_offset = 0;
                y_offset = 2;
                break;
            case 't':
            case 'T':
                x_offset = 1;
                y_offset = 2;
                break;
            case 'u':
            case 'U':
                x_offset = 2;
                y_offset = 2;
                break;
            case 'v':
            case 'V':
                x_offset = 3;
                y_offset = 2;
                break;
            case 'w':
            case 'W':
                x_offset = 4;
                y_offset = 2;
                break;
            case 'x':
            case 'X':
                x_offset = 5;
                y_offset = 2;
                break;
            case 'y':
            case 'Y':
                x_offset = 0;
                y_offset = 1;
                break;
            case 'z':
            case 'Z':
                x_offset = 1;
                y_offset = 1;
                break;
            case '1':
                x_offset = 2;
                y_offset = 1;
                break;
            case '2':
                x_offset = 3;
                y_offset = 1;
                break;
            case '3':
                x_offset = 4;
                y_offset = 1;
                break;
            case '4':
                x_offset = 5;
                y_offset = 1;
                break;
            case '5':
                x_offset = 0;
                y_offset = 0;
                break;
            case '6':
                x_offset = 1;
                y_offset = 0;
                break;
            case '7':
                x_offset = 2;
                y_offset = 0;
                break;
            case '8':
                x_offset = 3;
                y_offset = 0;
                break;
            case '9':
                x_offset = 4;
                y_offset = 0;
                break;
            case '0':
                x_offset = 5;
                y_offset = 0;
                break;
            case ' ':
            default:
                // space by default
                this->uvs.emplace_back(0.0f, 0.0f);
                this->uvs.emplace_back(0.04, 0.0);
                this->uvs.emplace_back(0.0f, 0.04f);
                this->uvs.emplace_back(0.04f, 0.04f);
                break;
        }

        this->uvs.emplace_back(x_offset / x_width, y_offset / y_width);
        this->uvs.emplace_back((x_offset + 1) / x_width, y_offset / y_width);
        this->uvs.emplace_back(x_offset / x_width, (y_offset + 1) / y_width);
        this->uvs.emplace_back((x_offset + 1) / x_width, (y_offset + 1) / y_width);
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
       static GLuint text_texture = loadTexture(
                "../textures/mythos_text_map.png",
                GL_LINEAR,
                GL_LINEAR,
                true
        );
        return text_texture;
    }





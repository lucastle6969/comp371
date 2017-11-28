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
#include <iostream>

TextB::TextB(
        const GLuint& shader_program,
        const std::string& message,
        const int line_start,
        const float left_margin,
        const unsigned int FONT_STYLE,
        Entity* parent
) : DrawableEntity(shader_program, parent) {

    this->draw_mode = GL_TRIANGLES;

    if(FONT_STYLE==FONT_STYLE_OUTLINE){
        this->font_map = loadTexture(
                "../textures/outline_text_map.png",
                GL_LINEAR,
                GL_LINEAR,
                true
        );
    }else{
        this->font_map = loadTexture(
                "../textures/mythos_text_map.png",
                GL_LINEAR,
                GL_LINEAR,
                true
        );
    }

    // 0.01 scale for at least 100 chars per line
    float char_width = 0.01;
    float char_height = 0.01;

    //just initializing for good measure (default)
    float current_char_r_space = 0.0f;
    float prev_char_r_space = 0.0f;
    float current_char_l_space = 0.0f;
    float prev_char_l_space = 0.0f;

    // keep track of the width to return line (line width sum accumulator)
    float cursor = left_margin;

    for(int i=0, j=0; i<message.length(); i++, j+=4){




        this->elements.emplace_back(j+2);
        this->elements.emplace_back(j);
        this->elements.emplace_back(j+1);
        this->elements.emplace_back(j+2);
        this->elements.emplace_back(j+1);
        this->elements.emplace_back(j+3);


        //we are not using these but want to keep a consistentency with drawable methods, and they could be used to light text
        this->normals.emplace_back(0.0f, 0.0f, 1.0f);
        this->normals.emplace_back(0.0f, 0.0f, 1.0f);
        this->normals.emplace_back(0.0f, 0.0f, 1.0f);
        this->normals.emplace_back(0.0f, 0.0f, 1.0f);


        /* could make 6 floats :
        float col_0 = 0.0f, col_1 = 1.0f/6.0f, col_2 = 2.0f/6.0f, col_3 = 0.5f, col_4 = 4.0f/6.0f, col_5 = 1.0f;
        float row_0 = 0.0f, row_1 = 1.0f/6.0f, row_2 = 2.0f/6.0f, row_3 = 0.5f, row_4 = 4.0f/6.0f, row_5 = 1.0f;
        */



        switch(message[i]){
            case 'a':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0, 0.833333);
                this->uvs.emplace_back(0.166666, 0.833333);
                this->uvs.emplace_back(0, 1);
                this->uvs.emplace_back(0.166666, 1);
                break;
            case 'b':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.166666, 0.833333);
                this->uvs.emplace_back(0.333333, 0.833333);
                this->uvs.emplace_back(0.166666, 1);
                this->uvs.emplace_back(0.333333, 1);
                break;
            case 'c':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;

                this->uvs.emplace_back(0.333333, 0.833333);
                this->uvs.emplace_back(0.5, 0.833333);
                this->uvs.emplace_back(0.333333, 1);
                this->uvs.emplace_back(0.5, 1);
                break;
            case 'd':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.5, 0.833333);
                this->uvs.emplace_back(0.666666, 0.833333);
                this->uvs.emplace_back(0.5, 1);
                this->uvs.emplace_back(0.666666, 1);
                break;
            case 'e':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.666666, 0.833333);
                this->uvs.emplace_back(0.833333, 0.833333);
                this->uvs.emplace_back(0.666666, 1);
                this->uvs.emplace_back(0.833333, 1);
                break;
            case 'f':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.833333, 0.833333);
                this->uvs.emplace_back(1, 0.833333);
                this->uvs.emplace_back(0.833333, 1);
                this->uvs.emplace_back(1, 1);
                break;
            case 'g':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0, 0.666666);
                this->uvs.emplace_back(0.166666, 0.666666);
                this->uvs.emplace_back(0, 0.833333);
                this->uvs.emplace_back(0.166666, 0.833333);
                break;
            case 'h':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.166666, 0.666666);
                this->uvs.emplace_back(0.333333, 0.666666);
                this->uvs.emplace_back(0.166666, 0.833333);
                this->uvs.emplace_back(0.333333, 0.833333);
                break;
            case 'i':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.333333, 0.666666);
                this->uvs.emplace_back(0.5, 0.666666);
                this->uvs.emplace_back(0.333333, 0.833333);
                this->uvs.emplace_back(0.5, 0.833333);
                break;
            case 'j':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.5, 0.666666);
                this->uvs.emplace_back(0.666666, 0.666666);
                this->uvs.emplace_back(0.5, 0.833333);
                this->uvs.emplace_back(0.666666, 0.833333);
                break;
            case 'k':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.666666, 0.666666);
                this->uvs.emplace_back(0.833333, 0.666666);
                this->uvs.emplace_back(0.666666, 0.833333);
                this->uvs.emplace_back(0.833333, 0.833333);
                break;
            case 'l':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.833333, 0.666666);
                this->uvs.emplace_back(1, 0.666666);
                this->uvs.emplace_back(0.833333, 0.833333);
                this->uvs.emplace_back(1, 0.833333);
                break;
            case 'm':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0, 0.5);
                this->uvs.emplace_back(0.166666, 0.5);
                this->uvs.emplace_back(0, 0.666666);
                this->uvs.emplace_back(0.166666, 0.666666);
                break;
            case 'n':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.166666, 0.5);
                this->uvs.emplace_back(0.333333, 0.5);
                this->uvs.emplace_back(0.166666, 0.666666);
                this->uvs.emplace_back(0.333333, 0.666666);
                break;
            case 'o':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.333333, 0.5);
                this->uvs.emplace_back(0.5, 0.5);
                this->uvs.emplace_back(0.333333, 0.666666);
                this->uvs.emplace_back(0.5, 0.666666);
                break;
            case 'p':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.5, 0.5);
                this->uvs.emplace_back(0.666666, 0.5);
                this->uvs.emplace_back(0.5, 0.666666);
                this->uvs.emplace_back(0.666666, 0.666666);
                break;
            case 'q':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.666666, 0.5);
                this->uvs.emplace_back(0.833333, 0.5);
                this->uvs.emplace_back(0.666666, 0.666666);
                this->uvs.emplace_back(0.833333, 0.666666);
                break;
            case 'r':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.833333, 0.5);
                this->uvs.emplace_back(1.0, 0.5);
                this->uvs.emplace_back(0.833333, 0.666666);
                this->uvs.emplace_back(1.0, 0.666666);
                break;
            case 's':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0, 0.333333);
                this->uvs.emplace_back(0.166666, 0.333333);
                this->uvs.emplace_back(0, 0.5);
                this->uvs.emplace_back(0.166666, 0.5);
                break;
            case 't':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.166666, 0.333333);
                this->uvs.emplace_back(0.333333, 0.333333);
                this->uvs.emplace_back(0.166666, 0.5);
                this->uvs.emplace_back(0.333333, 0.5);
                break;
            case 'u':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.333333, 0.333333);
                this->uvs.emplace_back(0.5, 0.333333);
                this->uvs.emplace_back(0.333333, 0.5);
                this->uvs.emplace_back(0.555555, 0.5);
                break;
            case 'v':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.5, 0.333333);
                this->uvs.emplace_back(0.666666, 0.333333);
                this->uvs.emplace_back(0.5, 0.5);
                this->uvs.emplace_back(0.666666, 0.5);
                break;
            case 'w':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.666666, 0.333333);
                this->uvs.emplace_back(0.833333, 0.333333);
                this->uvs.emplace_back(0.666666, 0.5);
                this->uvs.emplace_back(0.833333, 0.5);
                break;
            case 'x':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.833333, 0.333333);
                this->uvs.emplace_back(1, 0.333333);
                this->uvs.emplace_back(0.833333, 0.5);
                this->uvs.emplace_back(1, 0.5);
                break;
            case 'y':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0, 0.166666);
                this->uvs.emplace_back(0.166666, 0.166666);
                this->uvs.emplace_back(0, 0.333333);
                this->uvs.emplace_back(0.166666, 0.333333);
                break;
            case 'z':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.166666, 0.166666);
                this->uvs.emplace_back(0.333333, 0.166666);
                this->uvs.emplace_back(0.166666, 0.333333);
                this->uvs.emplace_back(0.333333, 0.333333);
                break;
            case '1':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.333333, 0.166666);
                this->uvs.emplace_back(0.5, 0.166666);
                this->uvs.emplace_back(0.333333, 0.333333);
                this->uvs.emplace_back(0.5, 0.333333);
                break;
            case '2':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.5, 0.166666);
                this->uvs.emplace_back(0.666666, 0.166666);
                this->uvs.emplace_back(0.5, 0.333333);
                this->uvs.emplace_back(0.666666, 0.333333);
                break;
            case '3':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.666666, 0.166666);
                this->uvs.emplace_back(0.833333, 0.166666);
                this->uvs.emplace_back(0.666666, 0.333333);
                this->uvs.emplace_back(0.833333, 0.333333);
                break;
            case '4':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.833333, 0.166666);
                this->uvs.emplace_back(1, 0.166666);
                this->uvs.emplace_back(0.833333, 0.333333);
                this->uvs.emplace_back(1, 0.333333);
                break;
            case '5':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0, 0.166666);
                this->uvs.emplace_back(0.166666, 0.166666);
                this->uvs.emplace_back(0, 0.333333);
                this->uvs.emplace_back(0.166666, 0.333333);
                break;
            case '6':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.166666, 0);
                this->uvs.emplace_back(0.333333, 0);
                this->uvs.emplace_back(0.166666, 0.166666);
                this->uvs.emplace_back(0.333333, 0.166666);
                break;
            case '7':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.333333, 0);
                this->uvs.emplace_back(0.5, 0);
                this->uvs.emplace_back(0.333333, 0.166666);
                this->uvs.emplace_back(0.5, 0.166666);
                break;
            case '8':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.5, 0);
                this->uvs.emplace_back(0.666666, 0);
                this->uvs.emplace_back(0.5, 0.166666);
                this->uvs.emplace_back(0.666666, 0.166666);
                break;
            case '9':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.666666, 0);
                this->uvs.emplace_back(0.833333, 0);
                this->uvs.emplace_back(0.666666, 0.166666);
                this->uvs.emplace_back(0.833333, 0.166666);
                break;
            case '0':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.833333, 0);
                this->uvs.emplace_back(1, 0);
                this->uvs.emplace_back(0.833333, 0.166666);
                this->uvs.emplace_back(1, 0.166666);
                break;
            case ' ':
                current_char_l_space = 0.0;//is this too much for a space ?
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.0f, 0.0f);
                this->uvs.emplace_back(0.04, 0.0);
                this->uvs.emplace_back(0.0f, 0.04f);
                this->uvs.emplace_back(0.04f, 0.04f);
                break;
            case ',':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                break;
            case '.':
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;
                this->uvs.emplace_back(0.0f, 0.0f);
                this->uvs.emplace_back(0.04, 0.0);
                this->uvs.emplace_back(0.0f, 0.04f);
                this->uvs.emplace_back(0.04f, 0.04f);
                break;
            default:
                current_char_l_space = 0.0;
                current_char_r_space = 0.0;

                this->uvs.emplace_back(0.0f, 0.0f);
                this->uvs.emplace_back(0.04, 0.0);
                this->uvs.emplace_back(0.0f, 0.04f);
                this->uvs.emplace_back(0.04f, 0.04f);

        }

        //we are 'cutting away' the extra space before and after chars
        cursor += char_width - (current_char_l_space + current_char_r_space);

        //we need to check if we need to go to the next line BEFORE assigning the vertices
        if(cursor > 1.0){
            cursor = 0.0f;

        }


        this->vertices.emplace_back(line_width_accumulator * var_char_width, line_height * char_height, 0.0f);
        this->vertices.emplace_back(i * var_char_width + var_char_width, line_height * char_height, 0.0f);
        this->vertices.emplace_back(i * var_char_width, line_height * char_height + char_height, 0.0f);
        this->vertices.emplace_back(i * var_char_width + var_char_width, line_height * char_height + char_height, 0.0f);


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
        return this->font_map;
    }




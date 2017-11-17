#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <stdexcept>

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Rock.hpp"

#include "../constants.hpp"


Rock::Rock(
    const GLuint &shader_program,
    const float &x_location,
    const float &z_location,
    const float &x_span,
    const float &z_span,
    Entity *parent
) : DrawableEntity(shader_program, parent)
{
    static const std::vector<GLuint> elements = {
        0, 6, 5, 0, 1, 6, 1, 7, 6, 1, 2, 7, 2, 8, 7, 2, 3, 8, 3, 9, 8, 3, 4, 9, 5, 11, 10, 5, 6, 11, 6, 12, 11, 6, 7, 12, 7, 13, 12, 7, 8, 13, 8, 14, 13, 8, 9, 14, 10, 16, 15, 10, 11, 16, 11, 17, 16, 11, 12, 17, 12, 18, 17, 12, 13, 18, 13, 19, 18, 13, 14, 19, 15, 21, 20, 15, 16, 21, 16, 22, 21, 16, 17, 22, 17, 23, 22, 17, 18, 23, 18, 24, 23, 18, 19, 24,


        0, 31, 30, 0, 5, 31, 5, 32, 31, 5, 10, 32, 10, 33, 32, 10, 15, 33, 15, 34, 33, 15, 20, 34, 30, 36, 35, 30, 31, 36, 31, 37, 36, 31, 32, 37, 32, 38, 37, 32, 33, 38, 33, 39, 38, 33, 34, 39, 35, 41, 40, 35, 36, 41, 36, 42, 41, 36, 37, 42, 37, 43, 42, 37, 38, 43, 38, 44, 43, 38, 39, 44, 40, 46, 45, 40, 41, 46, 41, 47, 46, 41, 42, 47, 42, 48, 47, 42, 43, 48, 43, 49, 48, 43, 44, 49,


        20, 56, 34, 20, 21, 56, 21, 57, 56, 21, 22, 57, 22, 58, 57, 22, 23, 58, 23, 59, 58, 23, 24, 59, 34, 61, 39, 34, 56, 61, 56, 62, 61, 56, 57, 62, 57, 63, 62, 57, 58, 63, 58, 64, 63, 58, 59, 64, 39, 66, 44, 39, 61, 66, 61, 67, 66, 61, 62, 67, 62, 68, 67, 62, 63, 68, 63, 69, 68, 63, 64, 69, 44, 71, 49, 44, 66, 71, 66, 72, 71, 66, 67, 72, 67, 73, 72, 67, 68, 73, 68, 74, 73, 68, 69, 74,


        4, 81, 80, 4, 9, 81, 9, 82, 81, 9, 14, 82, 14, 83, 82, 14, 19, 83, 19, 59, 83, 19, 24, 59, 80, 86, 85, 80, 81, 86, 81, 87, 86, 81, 82, 87, 82, 88, 87, 82, 83, 88, 83, 64, 88, 83, 59, 64, 85, 91, 90, 85, 86, 91, 86, 92, 91, 86, 87, 92, 87, 93, 92, 87, 88, 93, 88, 69, 93, 88, 64, 69, 90, 96, 95, 90, 91, 96, 91, 97, 96, 91, 92, 97, 92, 98, 97, 92, 93, 98, 93, 74, 98, 93, 69, 74,


        0, 106, 30, 0, 1, 106, 1, 107, 106, 1, 2, 107, 2, 108, 107, 2, 3, 108, 3, 80, 108, 3, 4, 80, 30, 111, 35, 30, 106, 111, 106, 112, 111, 106, 107, 112, 107, 113, 112, 107, 108, 113, 108, 85, 113, 108, 80, 85, 35, 116, 40, 35, 111, 116, 111, 117, 116, 111, 112, 117, 112, 118, 117, 112, 113, 118, 113, 90, 118, 113, 85, 90, 40, 121, 45, 40, 116, 121, 116, 122, 121, 116, 117, 122, 117, 123, 122, 117, 118, 123, 118, 95, 123, 118, 90, 95,


        45, 131, 46, 45, 121, 131, 121, 132, 131, 121, 122, 132, 122, 133, 132, 122, 123, 133, 123, 96, 133, 123, 95, 96, 46, 136, 47, 46, 131, 136, 131, 137, 136, 131, 132, 137, 132, 138, 137, 132, 133, 138, 133, 97, 138, 133, 96, 97, 47, 141, 48, 47, 136, 141, 136, 142, 141, 136, 137, 142, 137, 143, 142, 137, 138, 143, 138, 98, 143, 138, 97, 98, 48, 71, 49, 48, 141, 71, 141, 72, 71, 141, 142, 72, 142, 73, 72, 142, 143, 73, 143, 74, 73, 143, 98, 74
    };

    this->draw_mode = GL_TRIANGLES;
    this->setPosition(glm::vec3(x_location, 0.0f, z_location));

    //bottom face (5x5 between 0 - 1)
    this->vertices.emplace_back(0.00f, 0.00f, 0.00f);
    this->vertices.emplace_back(0.25f, 0.00f, 0.00f);
    this->vertices.emplace_back(0.50f, 0.00f, 0.00f);
    this->vertices.emplace_back(0.75f, 0.00f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.00f, 0.00f);

    this->vertices.emplace_back(0.00f, 0.00f, -0.25f);
    this->vertices.emplace_back(0.25f, 0.00f, -0.25f);
    this->vertices.emplace_back(0.50f, 0.00f, -0.25f);
    this->vertices.emplace_back(0.75f, 0.00f, -0.25f);
    this->vertices.emplace_back(1.00f, 0.00f, -0.25f);

    this->vertices.emplace_back(0.00f, 0.00f, -0.50f);
    this->vertices.emplace_back(0.25f, 0.00f, -0.50f);
    this->vertices.emplace_back(0.50f, 0.00f, -0.50f);
    this->vertices.emplace_back(0.75f, 0.00f, -0.50f);
    this->vertices.emplace_back(1.00f, 0.00f, -0.50f);

    this->vertices.emplace_back(0.00f, 0.00f, -0.75f);
    this->vertices.emplace_back(0.25f, 0.00f, -0.75f);
    this->vertices.emplace_back(0.50f, 0.00f, -0.75f);
    this->vertices.emplace_back(0.75f, 0.00f, -0.75f);
    this->vertices.emplace_back(1.00f, 0.00f, -0.75f);

    this->vertices.emplace_back(0.00f, 0.00f, -1.0f);
    this->vertices.emplace_back(0.25f, 0.00f, -1.0f);
    this->vertices.emplace_back(0.50f, 0.00f, -1.0f);
    this->vertices.emplace_back(0.75f, 0.00f, -1.0f);
    this->vertices.emplace_back(1.00f, 0.00f, -1.0f);

    //left (side) face
    this->vertices.emplace_back(0.00f, 0.00f, 0.00f);
    this->vertices.emplace_back(0.00f, 0.00f, -0.25f);
    this->vertices.emplace_back(0.00f, 0.00f, -0.50f);
    this->vertices.emplace_back(0.00f, 0.00f, -0.75f);
    this->vertices.emplace_back(0.00f, 0.00f, -1.00f);

    this->vertices.emplace_back(0.00f, 0.25f, 0.00f);
    this->vertices.emplace_back(0.00f, 0.25f, -0.25f);
    this->vertices.emplace_back(0.00f, 0.25f, -0.50f);
    this->vertices.emplace_back(0.00f, 0.25f, -0.75f);
    this->vertices.emplace_back(0.00f, 0.25f, -1.00f);

    this->vertices.emplace_back(0.00f, 0.50f, 0.00f);
    this->vertices.emplace_back(0.00f, 0.50f, -0.25f);
    this->vertices.emplace_back(0.00f, 0.50f, -0.50f);
    this->vertices.emplace_back(0.00f, 0.50f, -0.75f);
    this->vertices.emplace_back(0.00f, 0.50f, -1.00f);

    this->vertices.emplace_back(0.00f, 0.75f, 0.00f);
    this->vertices.emplace_back(0.00f, 0.75f, -0.25f);
    this->vertices.emplace_back(0.00f, 0.75f, -0.50f);
    this->vertices.emplace_back(0.00f, 0.75f, -0.75f);
    this->vertices.emplace_back(0.00f, 0.75f, -1.00f);

    this->vertices.emplace_back(0.00f, 1.00f, 0.00f);
    this->vertices.emplace_back(0.00f, 1.00f, -0.25f);
    this->vertices.emplace_back(0.00f, 1.00f, -0.50f);
    this->vertices.emplace_back(0.00f, 1.00f, -0.75f);
    this->vertices.emplace_back(0.00f, 1.00f, -1.00f);


    //back face (5x5 between 0 - 1)
    this->vertices.emplace_back(0.00f, 0.00f, -1.00f);
    this->vertices.emplace_back(0.25f, 0.00f, -1.00f);
    this->vertices.emplace_back(0.50f, 0.00f, -1.00f);
    this->vertices.emplace_back(0.75f, 0.00f, -1.00f);
    this->vertices.emplace_back(1.00f, 0.00f, -1.00f);

    this->vertices.emplace_back(0.00f, 0.25f, -1.00f);
    this->vertices.emplace_back(0.25f, 0.25f, -1.00f);
    this->vertices.emplace_back(0.50f, 0.25f, -1.00f);
    this->vertices.emplace_back(0.75f, 0.25f, -1.00f);
    this->vertices.emplace_back(1.00f, 0.25f, -1.00f);

    this->vertices.emplace_back(0.00f, 0.50f, -1.00f);
    this->vertices.emplace_back(0.25f, 0.50f, -1.00f);
    this->vertices.emplace_back(0.50f, 0.50f, -1.00f);
    this->vertices.emplace_back(0.75f, 0.50f, -1.00f);
    this->vertices.emplace_back(1.00f, 0.50f, -1.00f);

    this->vertices.emplace_back(0.00f, 0.75f, -1.00f);
    this->vertices.emplace_back(0.25f, 0.75f, -1.00f);
    this->vertices.emplace_back(0.50f, 0.75f, -1.00f);
    this->vertices.emplace_back(0.75f, 0.75f, -1.00f);
    this->vertices.emplace_back(1.00f, 0.75f, -1.00f);

    this->vertices.emplace_back(0.00f, 1.00f, -1.0f);
    this->vertices.emplace_back(0.25f, 1.00f, -1.0f);
    this->vertices.emplace_back(0.50f, 1.00f, -1.0f);
    this->vertices.emplace_back(0.75f, 1.00f, -1.0f);
    this->vertices.emplace_back(1.00f, 1.00f, -1.0f);

    //right (side) face
    this->vertices.emplace_back(1.00f, 0.00f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.00f, -0.25f);
    this->vertices.emplace_back(1.00f, 0.00f, -0.50f);
    this->vertices.emplace_back(1.00f, 0.00f, -0.75f);
    this->vertices.emplace_back(1.00f, 0.00f, -1.00f);

    this->vertices.emplace_back(1.00f, 0.25f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.25f, -0.25f);
    this->vertices.emplace_back(1.00f, 0.25f, -0.50f);
    this->vertices.emplace_back(1.00f, 0.25f, -0.75f);
    this->vertices.emplace_back(1.00f, 0.25f, -1.00f);

    this->vertices.emplace_back(1.00f, 0.50f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.50f, -0.25f);
    this->vertices.emplace_back(1.00f, 0.50f, -0.50f);
    this->vertices.emplace_back(1.00f, 0.50f, -0.75f);
    this->vertices.emplace_back(1.00f, 0.50f, -1.00f);

    this->vertices.emplace_back(1.00f, 0.75f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.75f, -0.25f);
    this->vertices.emplace_back(1.00f, 0.75f, -0.50f);
    this->vertices.emplace_back(1.00f, 0.75f, -0.75f);
    this->vertices.emplace_back(1.00f, 0.75f, -1.00f);

    this->vertices.emplace_back(1.00f, 1.00f, 0.00f);
    this->vertices.emplace_back(1.00f, 1.00f, -0.25f);
    this->vertices.emplace_back(1.00f, 1.00f, -0.50f);
    this->vertices.emplace_back(1.00f, 1.00f, -0.75f);
    this->vertices.emplace_back(1.00f, 1.00f, -1.00f);

    //front face (5x5 between 0 - 1)
    this->vertices.emplace_back(0.00f, 0.00f, 0.00f);
    this->vertices.emplace_back(0.25f, 0.00f, 0.00f);
    this->vertices.emplace_back(0.50f, 0.00f, 0.00f);
    this->vertices.emplace_back(0.75f, 0.00f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.00f, 0.00f);

    this->vertices.emplace_back(0.00f, 0.25f, 0.00f);
    this->vertices.emplace_back(0.25f, 0.25f, 0.00f);
    this->vertices.emplace_back(0.50f, 0.25f, 0.00f);
    this->vertices.emplace_back(0.75f, 0.25f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.25f, 0.00f);

    this->vertices.emplace_back(0.00f, 0.50f, 0.00f);
    this->vertices.emplace_back(0.25f, 0.50f, 0.00f);
    this->vertices.emplace_back(0.50f, 0.50f, 0.00f);
    this->vertices.emplace_back(0.75f, 0.50f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.50f, 0.00f);

    this->vertices.emplace_back(0.00f, 0.75f, 0.00f);
    this->vertices.emplace_back(0.25f, 0.75f, 0.00f);
    this->vertices.emplace_back(0.50f, 0.75f, 0.00f);
    this->vertices.emplace_back(0.75f, 0.75f, 0.00f);
    this->vertices.emplace_back(1.00f, 0.75f, 0.00f);

    this->vertices.emplace_back(0.00f, 1.00f, 0.0f);
    this->vertices.emplace_back(0.25f, 1.00f, 0.0f);
    this->vertices.emplace_back(0.50f, 1.00f, 0.0f);
    this->vertices.emplace_back(0.75f, 1.00f, 0.0f);
    this->vertices.emplace_back(1.00f, 1.00f, 0.0f);

    //top face (5x5 between 0 - 1)
    this->vertices.emplace_back(0.00f, 1.00f, 0.00f);
    this->vertices.emplace_back(0.25f, 1.00f, 0.00f);
    this->vertices.emplace_back(0.50f, 1.00f, 0.00f);
    this->vertices.emplace_back(0.75f, 1.00f, 0.00f);
    this->vertices.emplace_back(1.00f, 1.00f, 0.00f);

    this->vertices.emplace_back(0.00f, 1.00f, -0.25f);
    this->vertices.emplace_back(0.25f, 1.00f, -0.25f);
    this->vertices.emplace_back(0.50f, 1.00f, -0.25f);
    this->vertices.emplace_back(0.75f, 1.00f, -0.25f);
    this->vertices.emplace_back(1.00f, 1.00f, -0.25f);

    this->vertices.emplace_back(0.00f, 1.00f, -0.50f);
    this->vertices.emplace_back(0.25f, 1.00f, -0.50f);
    this->vertices.emplace_back(0.50f, 1.00f, -0.50f);
    this->vertices.emplace_back(0.75f, 1.00f, -0.50f);
    this->vertices.emplace_back(1.00f, 1.00f, -0.50f);

    this->vertices.emplace_back(0.00f, 1.00f, -0.75f);
    this->vertices.emplace_back(0.25f, 1.00f, -0.75f);
    this->vertices.emplace_back(0.50f, 1.00f, -0.75f);
    this->vertices.emplace_back(0.75f, 1.00f, -0.75f);
    this->vertices.emplace_back(1.00f, 1.00f, -0.75f);

    this->vertices.emplace_back(0.00f, 1.00f, -1.0f);
    this->vertices.emplace_back(0.25f, 1.00f, -1.0f);
    this->vertices.emplace_back(0.50f, 1.00f, -1.0f);
    this->vertices.emplace_back(0.75f, 1.00f, -1.0f);
    this->vertices.emplace_back(1.00f, 1.00f, -1.0f);

    for(int i=0; i<this->vertices.size(); i++){

        //random number between 0-1 , multiplied by 25 (the space between vertices is 0.25, - 13 (to have it go around zero
        // divided be 100 to put it back to 0.xx
        float jitterX = ((((float) rand())/(RAND_MAX))*25 - 13)/100;
        float jitterY = ((((float) rand())/(RAND_MAX))*25 - 13)/100;
        float jitterZ = ((((float) rand())/(RAND_MAX))*25 - 13)/100;

        this->vertices[i] = this->vertices[i] + glm::vec3(jitterX, jitterY, jitterZ);

    }

    this->vao = this->initVertexArray(
            this->vertices,
            elements,
            &this->vertices_buffer,
            &this->element_buffer
    );
}

const std::vector<glm::vec3>& Rock::getVertices()
{
    return this->vertices;
}

GLuint Rock::getVAO()
{
    return this->vao;
}

const int Rock::getColorType()
{
    return COLOR_HEIGHT;
}

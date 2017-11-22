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

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "RockB.hpp"

#include "../constants.hpp"
#include "../loadTexture.hpp"


RockB::RockB(
        const GLuint &shader_program,
        const float &world_x_location,
        const float &world_z_location,
        const float &x_span,
        const float &z_span,
        Entity *parent
) : DrawableEntity(shader_program, parent)
{
    this->draw_mode = GL_TRIANGLES;

    //generate a sphere
    //this could be useful to change with  +/-  rand() % 5
    int num_arc_points = 9 ;
    int num_longitude_lines = 8;
    generateSphere(&this->vertices, &this->elements, &this->normals, &this->uvs, (num_arc_points-1), num_longitude_lines);



    this->vao = DrawableEntity::initVertexArray(
            this->vertices,
            this->elements,
            this->uvs,
            &this->vertices_buffer,
            &this->element_buffer,
            &this->uv_buffer
    );
}

RockB::~RockB()
{
    glDeleteBuffers(1, &this->vertices_buffer);
    glDeleteBuffers(1, &this->element_buffer);
    glDeleteBuffers(1, &this->uv_buffer);
    glDeleteVertexArrays(1, &this->vao);
}

void RockB::generateSphere(
        std::vector<glm::vec3>* vertices,
        std::vector<GLuint>* ebo,
        std::vector<glm::vec3>* normals,
        std::vector<glm::vec2>* uvs,
        const int num_arc_segments,
        const int num_longitude_lines)
{

    //generate one arc with num_arc_segments
    for(float x=-1; x<=1; ){
        float y = sqrtf(1-x*x); //the equation of the arc (a perfect circle) can eventually be swapped out for any quadratic equation (hyperbolic arc)
        vertices->emplace_back(glm::vec3(x, y, 0));
        x+=2.0/num_arc_segments;
    }
    //we rotate the arc around the x-axis for every num_longitude_lines, -1 to connect the penultimate to the origin.
    //we duplicate the end-points in every rotation but then ignore all but the originals for the indicies (element) buffer
    for(int i=1; i<num_longitude_lines; i++){
        for(int j = 0; j<(num_arc_segments+1); j++) {
            vertices->push_back(glm::rotateX((*vertices)[j], glm::radians((360.0f/(float)num_longitude_lines)*i)));
        }
    }

    for(int l=0; l<(num_longitude_lines-1); l++) {
        int arc_offset = l*(num_arc_segments+1);
        for (int k = 1; k < (num_arc_segments-1); k++) {
            ebo->emplace_back(arc_offset+k);
            ebo->emplace_back(arc_offset+k + 1);
            ebo->emplace_back(arc_offset+k + num_arc_segments+1);
            ebo->emplace_back(arc_offset+k + 1);
            ebo->emplace_back(arc_offset+k + 1 + num_arc_segments+1);
            ebo->emplace_back(arc_offset+k + num_arc_segments+1);
        }
    }
    //join the last arc to the initial arc
    for (int m = 1; m < (num_arc_segments-1); m++){
        int arc_offset = (num_longitude_lines-1)*(num_arc_segments+1);
        ebo->emplace_back(arc_offset+m);
        ebo->emplace_back(arc_offset+m+1);
        ebo->emplace_back(m);
        ebo->emplace_back(arc_offset+m+1);
        ebo->emplace_back(m + 1);
        ebo->emplace_back(m);
    }
    //join 'top' pole
    for(int n = 1; n<num_longitude_lines; n++){
        int arc_offset = (n-1)*(num_arc_segments+1);
        ebo->emplace_back(0);
        ebo->emplace_back(arc_offset+1);
        ebo->emplace_back(arc_offset+1+num_arc_segments+1);
    }

    //join last triangle of top pole
    ebo->emplace_back(0);
    ebo->emplace_back((num_longitude_lines-1)*(num_arc_segments+1)+1);
    ebo->emplace_back(1);

    //join 'bottom' pole
    for(int n = 1; n<num_longitude_lines; n++){
        int arc_offset = (n-1)*(num_arc_segments+1);
        ebo->emplace_back(arc_offset+1+(num_arc_segments-2));
        ebo->emplace_back(num_arc_segments);//end point
        ebo->emplace_back(arc_offset+1+(num_arc_segments-2)+num_arc_segments+1);
    }

    //join last triangle of bottom pole
    ebo->emplace_back((num_longitude_lines-1)*(num_arc_segments+1)+1+(num_arc_segments-2));
    ebo->emplace_back(num_arc_segments);
    ebo->emplace_back(num_arc_segments-1);

    //normals and uvs (is emplace or push_back better here ?)
    for(int i = 0; i<vertices->size(); i++){
        uvs->emplace_back(
                //u coordinate determined by averaging out y and z (then bring the range between 0 - 1)
                ( ( (*vertices)[i].y + (*vertices)[i].z ) / 2.0f ) * 0.5f + 0.5f ,
                //v coordinate is basically the x component
                (*vertices)[i].x * 0.5f + 0.5f);

        normals->emplace_back((*vertices)[i]); //the normals are simply the vertices since this is a sphere around the origin
    }

}


const std::vector<glm::vec3>& RockB::getVertices()
{
    return this->vertices;
}

GLuint RockB::getVAO()
{
    return this->vao;
}

const int RockB::getColorType()
{
    return COLOR_TEXTURE;
}

GLuint RockB::getTextureId()
{
    static GLuint rock_texture = loadTexture(
            "../textures/stoneC.jpg",
            GL_LINEAR,
            GL_LINEAR
    );
    return rock_texture;
}

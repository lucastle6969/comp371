#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Tree.hpp"
#include "../constants.hpp"

/*Tree::Tree(
		const GLuint &shader_program,
		const float& world_x_location,
		const float& world_z_location,
		const float& base_span,
		Entity *parent
) : DrawableEntity(shader_program, parent) {
	// rectangular prism
	static const std::vector<GLuint> elements = {
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			0, 4,
			1, 5,
			2, 6,
			3, 7
	};

	this->draw_mode = GL_LINES;

	float height = 0.1f; // arbitrary height

	this->vertices = {
			glm::vec3(0.0f,      0.0f,   0.0f),      // 0: bottom-front-left
			glm::vec3(base_span, 0.0f,   0.0f),      // 1: bottom-front-right
			glm::vec3(base_span, 0.0f,   base_span), // 2: bottom-back-right
			glm::vec3(0.0f,      0.0f,   base_span), // 3: bottom-back-left
			glm::vec3(0.0f,      height, 0.0f),      // 4: top-front-left
			glm::vec3(base_span, height, 0.0f),      // 5: top-front-right
			glm::vec3(base_span, height, base_span), // 6: top-back-right
			glm::vec3(0.0f,      height, base_span)  // 7: top-back-left
	};

	this->vao = this->initVertexArray(this->vertices, elements);
}


*/


//
// Created by Tof on 2017-11-19.
//

#include "Tree.hpp"


const std::vector<glm::vec3>& Tree::getVertices() {
    return reinterpret_cast<const std::vector<glm::vec3> &>(combinedVertices);
}

GLuint Tree::getVAO() {
    return this->vao;
}

const int Tree::getColorType() {
    return COLOR_WHITE;
}


glm::vec3 Tree::boostSegment(AttatchmentGroupings* agLow, AttatchmentGroupings* agHigh, std::vector<glm::vec3>* vPntr){
    glm::vec3 AB = vPntr->at(agLow->end) - vPntr->at(agLow->end -1);
    glm::vec3 AC = vPntr->at(agLow->end-2) - vPntr->at(agLow->end -1);
    glm::vec3 low =  glm::normalize(glm::cross(AB, AC));
    glm::vec3 KJ = vPntr->at(agHigh->start + 1) - vPntr->at(agHigh->start+2);
    glm::vec3 KL = vPntr->at(agHigh->start + 3) - vPntr->at(agHigh->start+2);
    glm::vec3 high =  -glm::normalize(glm::cross(KJ, KL));
    return low + high;
}

//values in radians
glm::vec3 Tree::makeRotations(float xRot, float yRot, float zRot, glm::vec3 vector){
    //ROTATION ABOUT X
    // std::cout<< xRot << " "<< yRot << " " << zRot <<"\n" << vector.x <<" " << vector.y << " " << vector.z << "\n";
    vector = glm::vec3(
            vector.x
            , vector.y * cos(xRot) - vector.z * sin(xRot)
            ,	vector.y * sin(xRot) + vector.z * cos(xRot));
    //ROTATION ABOUT Z
    // << xRot << " "<< yRot << " " << zRot <<"\n" << vector.x <<" " << vector.y << " " << vector.z << "\n";
    vector = glm::vec3(
            vector.x * cos(zRot) - vector.y * sin(zRot)
            , vector.x * sin(zRot) + vector.y * cos(zRot)
            ,	vector.z );
    //ROTATION ABOUT Y
    // << xRot << " "<< yRot << " " << zRot <<"\n" << vector.x <<" " << vector.y << " " << vector.z << "\n";
    vector = glm::vec3(
            vector.x * cos(yRot) + vector.z * sin(yRot)
            , vector.y
            ,	-(vector.x * sin(yRot)) + vector.z * cos(yRot));
    // << xRot << " "<< yRot << " " << zRot <<"\n" << vector.x <<" " << vector.y << " " << vector.z << "\n-------\n";
    return vector;
}

//TODO: COMPLETE CONECTORS
void Tree::connectSegments(AttatchmentGroupings* ag, int m, int rotPoint, int prevPoint, int circularPoints,
                     std::vector<GLuint>* indPntr){
    int set = abs((circularPoints - rotPoint) + prevPoint);
    TrunkA::buildConnectorElements(ag->end - TrunkA::trunkPoints + 1, ag->ag[m]->start + 1, set, ag->side,
                                   combinedIndices, combinedVertices, combinedUV, combinedNormals);
}

void Tree::computeElementsInitial(AttatchmentGroupings* ag) {
    if(ag->type == 'L'){
        LeafContainerA::buildLeafContainerElements(ag->start + 1, ag->end,
                                                   combinedIndices, combinedVertices, combinedUV, combinedNormals);
    }
    else
        TrunkA::buildTrunkElements(ag->start + 1, ag->end,
                                   combinedIndices, combinedVertices, combinedUV, combinedNormals);
}


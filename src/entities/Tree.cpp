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

const std::vector<glm::vec3>& Tree::getVertices() {
    return reinterpret_cast<const std::vector<glm::vec3> &>(combinedVertices);
}

GLuint Tree::getVAO() {
    return this->vao;
}

const int Tree::getColorType() {
    return COLOR_TREE;
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
    vector = glm::vec3(
            vector.x
            , vector.y * cos(xRot) - vector.z * sin(xRot)
            ,	vector.y * sin(xRot) + vector.z * cos(xRot));
    //ROTATION ABOUT Z
    vector = glm::vec3(
            vector.x * cos(zRot) - vector.y * sin(zRot)
            , vector.x * sin(zRot) + vector.y * cos(zRot)
            ,	vector.z );
    //ROTATION ABOUT Y
    vector = glm::vec3(
            vector.x * cos(yRot) + vector.z * sin(yRot)
            , vector.y
            ,	-(vector.x * sin(yRot)) + vector.z * cos(yRot));

    return vector;
}

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


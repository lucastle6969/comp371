
#ifndef PROCEDURALWORLD_l_H
#define PROCEDURALWORLD_l_H

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/OpenGL.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


#include "src/TreeRandom.hpp"

class LeafContainerC {
    std::vector<glm::vec3>* combinedVertices;
    std::vector<GLuint>* combinedIndices;

    int lineMax;
    int heightChunking;
    int leafPoints = 4;
    float jagednessFactor_Leaf;
    int branchMod;
public:
    LeafContainerC(std::vector<glm::vec3>* combinedVertices, std::vector<GLuint>* combinedIndices,
           float lineMax);

    void buildAllComponenets(const float& leafDiameter, const float& seed, float lineHeight);
};


#endif //PROCEDURALWORLD_l_H

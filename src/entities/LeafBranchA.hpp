
#ifndef PROCEDURALWORLD_LEAFB_H
#define PROCEDURALWORLD_LEAFB_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../TreeRandom.hpp"
#include <cstdio>
class LeafBranchA {
    int randomSeedValue; float itterations; int branchPoints; float trunkDiameter;
    float lineHeight; float jagednessFactor_Leaf; std::vector<glm::vec3>* leafVertices;
    std::vector<GLuint>* leafIndices; std::vector<glm::vec2>* leafUVs;
public:
    float getLineHeight(){return lineHeight;}

    explicit LeafBranchA(int randomSeedValue, float itterations, int branchPoints,
                float lineHeight, float jagednessFactor_Leaf, 
                std::vector<glm::vec3>* leafVertices, 
                         std::vector<GLuint>* leafIndices, std::vector<glm::vec2>* leafUVs){
        this->randomSeedValue = randomSeedValue;this->itterations = itterations;this->branchPoints = branchPoints;
        this->trunkDiameter = trunkDiameter; this->lineHeight = lineHeight;this->jagednessFactor_Leaf = jagednessFactor_Leaf;
        this->leafVertices = leafVertices; this->leafIndices = leafIndices; this->leafUVs = leafUVs;
    }

    void buildLeafBranch(float trunkDiameter);
    
    void buildBranchElements(int baseVerticesSize, int i, int len);
};
#endif //leafBranch
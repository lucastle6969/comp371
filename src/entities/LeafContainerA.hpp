//
// Created by Tof on 2017-11-19.
//

#ifndef PROCEDURALWORLD_LEAFCONTAINER_H
#define PROCEDURALWORLD_LEAFCONTAINER_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "LeafA.hpp"
#include "LeafBranchA.hpp"
#include "TrunkA.hpp"
#include "../TreeRandom.hpp"

#define BRANCH_POLYGON 4
#define LEAF_POLYGON 4

class LeafContainerA {
public:
    static constexpr int leafPoints = LEAF_POLYGON;
    static constexpr int leafBranchPoints = BRANCH_POLYGON;
    const glm::vec3 green = glm::vec3(30.0 / 255, 147.0 / 255, 45.0 / 255);
    const float itterationsLeaf = 360.0 / leafPoints;
    const float itterationsLeafBranch = 360.0 / leafBranchPoints;

    static constexpr float jagednessFactor_Leaf = 0.35;
    static constexpr float textureLeafStart = 0.9f;
    static constexpr float textureLeafEnd = 1.0f;
private:
    std::vector<glm::vec3>* leafVertices; 	std::vector<GLuint>* leafIndices;
    std::vector<glm::vec3>* leafColor; std::vector<std::vector<int>>* leafStartIndices;
    std::vector<glm::vec2>* leafUVs;

    float lineHeight = 0;
    int baseVerticesSize = 0;
    int heightCount = 0;
    int seed = 0;
public:
    LeafContainerA(std::vector<glm::vec3>* leafVertices,
    std::vector<GLuint>* leafIndices,
            std::vector<glm::vec3>* leafColor,
    std::vector<std::vector<int>>* leafStartIndices,
            std::vector<glm::vec2>* leafUVs,
    int seed
    ){
        this->leafVertices = leafVertices;
        this->leafIndices = leafIndices;
        this->leafColor = leafColor;
        this->leafStartIndices = leafStartIndices;
        this->leafUVs = leafUVs;
        baseVerticesSize = leafVertices->size();
        this->seed = seed;
    }

void buildVertices(int randomSeedValue, float lineSegments, int lineMax, float trunkDiameter, float seed,
                   float lineHeight, int* baseVerticesSize);
  
    void buildContainer(float trunkDiameter, float seed, float lineHeight, int lineMax);
    
    static void buildLeafContainerElements(int start, int end,
                                           std::vector<GLuint>* leafIndices, std::vector<glm::vec3>* leafVert,
                                           std::vector<glm::vec2>* leafUVs, std::vector<glm::vec3>* leafNorms);
};


#endif //PROCEDURALWORLD_LEAFCONTAINER_H

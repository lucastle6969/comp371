
#ifndef PROCEDURALWORLD_LEAF_H
#define PROCEDURALWORLD_LEAF_H

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

class LeafA {
    int leafPoints; int randomSeedValue; int count;float lineHeight;float itterations ;
    float jagednessFactor_Leaf; std::vector<glm::vec3>* leafVertices;
    std::vector<GLuint>* leafIndices; std::vector<glm::vec2>* leafUVs;
public:
    explicit LeafA(int leafPoints, int randomSeedValue, int count,float lineHeight,float itterations ,
          float jagednessFactor_Leaf, std::vector<glm::vec3>* leafVertices,
                   std::vector<GLuint>* leafIndices, std::vector<glm::vec2>* leafUVs){
        this->leafPoints = leafPoints; this->randomSeedValue = randomSeedValue; this->count = count;this->lineHeight = lineHeight;
        this->itterations = itterations;  this->jagednessFactor_Leaf = jagednessFactor_Leaf; this->leafVertices = leafVertices;
    };

    //builds 2
    void buildLeaf(float r1, float r2, float leafDiameter);

    static int buildElements(int i, int start,  glm::vec3 leafNormal, int leafPoints ,  std::vector<GLuint>* leafIndices, std::vector<glm::vec3>* leafVert,
                             std::vector<glm::vec2>* leafUVs, std::vector<glm::vec3>* leafNorms);

    void buildLeafSingle(float r1, float r2, float trunkDiameter);
};
#endif //leaf
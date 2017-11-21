#ifndef COMP371_TRUNK_A_HPP
#define COMP371_TRUNK_A_HPP
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

#define BRANCH_MODULO 10
#define TRUNK_POLYGON 4

#include <iostream>
#include <cstdio>
#include <ctime>

class TrunkA{
public:
    const int branchMod = BRANCH_MODULO;
    static constexpr int trunkPoints = TRUNK_POLYGON;

    const glm::vec3 brown = glm::vec3(0.0 / 255, 53.0 / 255, 10.0 / 255);
    const float itterations = 360.0 / trunkPoints;

    static constexpr float jagednessFactor = 0.35;
    static constexpr float textureTrunkHeight = 0.75;
    static constexpr float textureConnectorStart = 0.75;
    static constexpr float textureConnectorEnd = 0.90;
private:
    //MOVE TO TRUNK CLASS
    std::vector<glm::vec3>* trunkVertices;
    std::vector<GLuint>* trunkIndices;
    std::vector<glm::vec3>* trunkColor;
    std::vector<std::vector<int>>* trunkStartIndices;
    std::vector<glm::vec2>* trunkUVs;

    float lineHeight = 0;
    int baseVerticesSize = 0;
    int heightCount = 0;
    int seed = 0;

    int buildVertices(float trunkDiameter, float lineSegments);

public:

    float getLineHeight();

    TrunkA(std::vector<glm::vec3>* trunkVertices,
          std::vector<GLuint>* trunkIndices,
          std::vector<glm::vec3>* trunkColor,
          std::vector<std::vector<int>>* trunkStartIndices,
          std::vector<glm::vec2>* trunkUVs,
           int seed
    ){
        this->trunkVertices = trunkVertices;
        this->trunkIndices = trunkIndices;
        this->trunkColor = trunkColor;
        this->trunkStartIndices = trunkStartIndices;
        this->trunkUVs = trunkUVs;
        baseVerticesSize = trunkVertices->size();
        this->seed = seed;
    }

    static void buildConnectorElements(int segmentConnectStart,int start, int set, char lr,
                                       std::vector<GLuint>* trunkIndices, std::vector<glm::vec3>* trunkVert,
                                       std::vector<glm::vec2>* trunkUVs, std::vector<glm::vec3>* trunkNorms);

    //give both start point and end as the connection end point
    static void buildTrunkElements(int start, int end,
                                   std::vector<GLuint>* trunkIndices, std::vector<glm::vec3>* trunkVert,
                                   std::vector<glm::vec2>* trunkUVs, std::vector<glm::vec3>* trunkNorms);
    
    bool buildTrunk(float trunkDiameter, float lineSegments);
};
#endif //COMP371_TRUNK_A_HPP
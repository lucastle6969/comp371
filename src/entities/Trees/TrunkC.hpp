
#ifndef PROCEDURALWORLD_TRUNKC_H
#define PROCEDURALWORLD_TRUNKC_H

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

class TrunkC {
    std::vector<glm::vec3>* combinedVertices;
    std::vector<GLuint>* combinedIndices;

    static constexpr int heightChunking = 20;
    static constexpr int trunkPoints = 4;
    static constexpr int branchMod  = 10;

    float jagednessFactor;
    float lineMax;
public:
    TrunkC(std::vector<glm::vec3>* combinedVertices, std::vector<GLuint>* combinedIndices,
           float lineMax);
    float buildAllComponents(const float& trunkDiameter, const float& seed, float lineHeight);
};


#endif //PROCEDURALWORLD_TRUNKC_H

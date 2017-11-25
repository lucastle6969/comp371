//
// Created by Scherezade on 24/11/2017.
//

#ifndef PROCEDURALWORLD_TRUNK_H
#define PROCEDURALWORLD_TRUNK_H

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

#include <iostream>
#include <cstdio>
#include <ctime>

class Trunk {
public:
    static void computeUVs(int end , int start, int trunkPoints, float textureTrunkHeight, std::vector<glm::vec2>* trunkUVs);
    static void computeUVsConnectors(int start, int trunkPoints, float textureTrunkHeight, std::vector<glm::vec2>* trunkUVs);
};


#endif //PROCEDURALWORLD_TRUNK_H

//
// Created by Tof on 2017-11-20.
//

#ifndef PROCEDURALWORLD_TREEDISTRIBUTOR_H
#define PROCEDURALWORLD_TREEDISTRIBUTOR_H


#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include "entities/Tree.hpp"
#include "entities/TreeA.hpp"
#include "src/entities/TreeB.hpp"
#include "src/entities/TreeC.hpp"
#include<exception>


class TreeDistributor {
    static int seed;
    static bool ready;
    static GLuint& shader_programA, shader_programB, shader_programC;
    static Entity* entity;
public:

    static void init(GLuint& shader_programA,GLuint&  shader_programB,GLuint&  shader_programC,
                     Entity* entity);

    static void setEntity(Entity* entity);

    static Tree* setTreeType(int seed, float width);
};


#endif //PROCEDURALWORLD_TREEDISTRIBUTOR_H

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

#include"Entities/Tree.hpp"
#include "Entities/TreeA.hpp"
#include "Entities/TreeB.h"
#include "Entities/TreeC.h"

class TreeDistributor {
    static int seed;
    static bool ready = false;
public:
    static GLuint& shader_programA = shader_programA, shader_programB, shader_programC;
    static Entity* entity;
    static void init(GLuint& shader_programA,GLuint&  shader_programB,GLuint&  shader_programC,
                     Entity* entity
    ){
        TreeDistributor::ready = true;
        TreeDistributor::shader_programA = shader_programA, TreeDistributor::shader_programB = shader_programB,
                TreeDistributor::shader_programC = shader_programC;
        TreeDistributor::entity = entity;
    }

    static Tree setTreeType(int seed, float width) throw()
            : std::runtime_error("DISTRIBUTOR NOT INITIALIZED"
                                         " -- CALL\"static void init(GLuint& shader_programA,GLuint&  "
                                         "shader_programB,GLuint&  shader_programC, Entity* entity){\""){
        if(!ready) throw;
        TreeDistributor::seed = seed;
        if(TreeDistributor::seed % 3 == 0){
            TreeA tA = TreeA(shader_programA, entity, width, seed);
            return tA;
        }
        else if(TreeDistributor::seed % 3 == 1){
            TreeB tB = TreeB(shader_programB, entity, width, seed);
            return tB;
        }
        else{
            TreeC tc = TreeC(seed % 5 + 1, shader_programB, entity, width, seed);
            return tc;
        }
    }
};


#endif //PROCEDURALWORLD_TREEDISTRIBUTOR_H

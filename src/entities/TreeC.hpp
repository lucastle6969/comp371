
/*
TODO: READ NOTATION OF TODO, CONSIDERATION AND SPLIT FOR OBJECTIVES. MASTER TODO- SPLIT ALL CLASSES FROM INLINE TO CPP HPP COMBINATION.
____DOCUMENTATION_OF_TREE_GENERATION_____
ONE CENTRAL .CPP FILE CONTROLS GENERATION OF TREE STRUCTURE. IS BROKEN DOWN INTO A LOG COMPONENT AND A LEAF BRANCH COMPONENT THAT
ARE MESHED TOGETHER USING A RECURISIVE ALGORITHM STORE COMBINED VERTICES(THROUGH POINTERS)
	TreeA(const GLuint& shader_program, float trunkDiameter, float seed)
CALL TO GENERATE OBJECT WILL RESULT IN AN OBJECT WITH A STORED VAO IN THESE PARAMETERS.
CREATES TRUNK AND LEAF VECTORS+INDICES WHICH ARE MERGED TOGETHER ARE SENT TO THE GPU.
CURRENT LOADING SPEED SUGESTS THE 4-8 DIAMETER RANGE WILL GENERATE QUICKLY. SCALING TO HEIGHTS MAY BE NESCICARY.
PROGRAM ENTRY POINT IN IN THE generateTreeA FUNCTION WHEREIN IT STARTS TO BUILD THE STARTER TRUNK,
THEN ENTERS BINARY RECURSIONS OF ITSELF TO GENERATE THE TRUNKS AND LEAVES WHEREUPON REACHING THE BASE CASE IN MOVES SEGMENTS OF
ONE RECURSION ONTO THE STARTER TRUNK AND PROCEEDS TO DO THE NEXT BRANCH. RECURSIONS SET PARAMETERS OF ANGLES AND SEGMENT WIDTHS
DECREASING AT A PYTHAGOREAN RATE.
TRUNKS HAVE FOUR PROPERTIES.
  1) THE WIDTH OF THE SEGMENT
  2) THE JAGEDNESS(BUMPYNESS) OF THE SEGMENT
  3) THE MAX HEIGHT IT CAN REACH
  4) THE SEED RELATED TO IT
THEY ARE STRAIGHT LINE PIECES OF ROUGHLY CIRCULAR SIZE

LEAF BRANCHES HAVE FOUR PROPERTIES
  1) THE WIDTH OF THE BRANCH SEGMENT
  2) THE JAGEDNESS(BUMPYNESS) OF THE SEGMENT'S LEAVES
  3) THE MAX HEIGHT IT CAN REACH
  4) THE SEED RELATED TO IT
AT EVERY PART OF THE BRANCH IT SPLITS OFF INTO TWO LEAVES ALTERNATING IN PERPENDICULARS.
AT THE LAST 2 PARTS THE LEAVES GET SMALLER UNTIL THE LAST WHERE IT ENDS WITH A LEAF.
LEAFS ARE DOUBLE SIDED FOR BACKFACE CULLING.
SEGMENTS ARE STORED INTO ATTATCHMENT GROUPINGS AS A LINKED LIST AND ARE STICHED TOGETHER IN THE MOVE PROCESS USING THE
ANGLES COMPUTED DURING RECURSIONS
*/

#ifndef COMP371_TREE_C_HPP
#define COMP371_TREE_C_HPP

#define START_TRUNK 0
#define TRUNK 2
#define LEAF 3
#define END_TRUNK -1

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <iostream>
#include <cstdio>
#include <ctime>


#include "DrawableEntity.hpp"
#include "Tree.hpp"
#include "TreeClusterItem.hpp"
#include "../TreeRandom.hpp"

class TreeC: public Tree{
private:
    std::vector<TreeClusterItem*> treeCluster;
    int spacingConstant = 10;
    float reductionRate = 0.5;
    static constexpr int heightChunking = 0;
    static constexpr int boostFactor = 0;
    int maxWidth(float trunkDiameter){
        return (int)(pow(spacingConstant * (trunkDiameter+1), 1.0/2) + trunkDiameter);
    }
public:
    TreeC(int numberOfTrees, const GLuint& shader_program, Entity* entity, float trunkDiameter, long seed):
            Tree(heightChunking, boostFactor, shader_program, entity, 'C'){
        double duration;
            std::clock_t startTime;
        startTime = std::clock();

        //center piece
        TreeClusterItem* tci = new TreeClusterItem(shader_program, this, trunkDiameter, seed);
        tci->setLocationWithPoints(0, 0);
        tci->setPosition(glm::vec3(tci->xPos, 0, tci->zPos));
        treeCluster.push_back(tci);
        numberOfTrees--;

        //distribute in random cirlce
        float tempTrunkDiameter = trunkDiameter;
        int i = 0;
        for (; i < numberOfTrees; i++) {
            reductionRate = 4.5f;//TreeRandom::middleSquareRange(seed, 4, 1) / 10.0f;
            seed = (unsigned long)(pow(seed, 2) * (tempTrunkDiameter+1)) % 99991;

            float circleAngle = TreeRandom::middleSquareRange(seed, 360.0, 0.0);
            float distanceFromCenter = TreeRandom::middleSquareRange(seed, maxWidth(trunkDiameter)*0, trunkDiameter*0);

            //std::cout << "DfC" << distanceFromCenter << "\n";
            float distScale = 30.0f;
            float xPos = cos(glm::radians(circleAngle)) * distanceFromCenter * distScale;
            float zPos =  sin(glm::radians(circleAngle)) * distanceFromCenter * distScale;

            //std::cout << "   " << tempTrunkDiameter << "\n";
            TreeClusterItem* tci = new TreeClusterItem(shader_program, this,
                                                           tempTrunkDiameter <= 0? 0:  tempTrunkDiameter -= reductionRate,
                                                           seed);
//            std::cout << xPos << " " << zPos << "\n";
            tci->setLocationWithPoints(xPos , zPos);
            tci->setPosition(glm::vec3(tci->xPos, 0, tci->zPos));
            //translate(glm::vec3(xPos, 0, zPos));
            treeCluster.push_back(tci);
        }

        duration = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
        printf("Duration of C %f Units: %f ms _CLUSTER HEAD\n", trunkDiameter, duration*1000);
    }


    std::vector<TreeClusterItem*> getTreeCluster(){
        return treeCluster;
    }

    void setReductionRate(float r){
        reductionRate = r;
    }

    ~TreeC(){
        for(TreeClusterItem* tci: treeCluster){
            delete tci;
        }
    }
};
//
//glm::vec3 U = leafVertices.at(base + n+1) - leafVertices.at(base + n);
//glm::vec3 V = leafVertices.at(offset + n) - leafVertices.at(base + n);
//glm::vec3 normal = glm::normalize(glm::cross(U, V));
//combinedNormals.push_back(normal);
#endif
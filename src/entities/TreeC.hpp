
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
        TreeClusterItem* tci = new TreeClusterItem(shader_program, entity, trunkDiameter, seed);
        tci->setLocationFromCenter(0, 0);
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
            float xPos = cos(glm::radians(circleAngle)) * distanceFromCenter;
            float zPos =  sin(glm::radians(circleAngle)) * distanceFromCenter;

            //std::cout << "   " << tempTrunkDiameter << "\n";
            TreeClusterItem* tci = new TreeClusterItem(shader_program, entity,
                                                           tempTrunkDiameter <= 0? 0:  tempTrunkDiameter -= reductionRate,
                                                           seed);
//            std::cout << xPos << " " << zPos << "\n";
            float distScale = 30.0f;
            tci->setLocationWithPoints(xPos / distScale, zPos / distScale);
            treeCluster.push_back(tci);
        }

        duration = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
        printf("Duration of C %f Units: %f ms _CLUSTER HEAD\n", trunkDiameter, duration*1000);
    }


    std::vector<TreeClusterItem*> getTreeCluster(){
        return treeCluster;
    }

    void scale(const float& scalar)
    {
        for(TreeClusterItem* tci : treeCluster){
            tci->scale(scalar);
        }
    }
    void rotate(const float& angle, const glm::vec3& axis)
    {
        for(TreeClusterItem* tci : treeCluster){
            tci->rotate( angle, axis);
        }
    }
    void resetRotation(){
        for(TreeClusterItem* tci : treeCluster){
            tci->resetRotation();
        }
    }
    void moveForward(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f){
        for(TreeClusterItem* tci : treeCluster){
            tci->moveForward(view_vec, up_vec,units);
        }
    }
    void moveBack(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f){
        for(TreeClusterItem* tci : treeCluster){
            tci->moveBack(view_vec,up_vec, units);
        }
    }
    void moveLeft(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f){
        for(TreeClusterItem* tci : treeCluster){
            tci->moveLeft(view_vec, up_vec,  units);
        }
    }
    void moveRight(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units){
        for(TreeClusterItem* tci : treeCluster){
            tci->moveRight(view_vec, up_vec, units);
        }
    }
    void translate(const glm::vec3& translation_vec){
        for(TreeClusterItem* tci : treeCluster){
            tci->translate(translation_vec);
        }
    }
    void setPosition(const glm::vec3& position){
        for(TreeClusterItem* tci : treeCluster){
            glm::vec3 localSpacing(tci->xPos, 0.0f, tci->zPos);
            tci-> setPosition(position + localSpacing);
        }
    }
    void hide(){
        for(TreeClusterItem* tci : treeCluster){
            tci->hide();
        }
    }
    void unhide(){
        for(TreeClusterItem* tci : treeCluster){
            tci->unhide();
        }
    }
    void toggleHide(){
        for(TreeClusterItem* tci : treeCluster){
            tci->toggleHide();
        }
    }
    void draw(const glm::mat4& view_matrix, const glm::mat4& projection_matrix){
        for(TreeClusterItem* tci : treeCluster){
            tci->draw(view_matrix, projection_matrix);
        }
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
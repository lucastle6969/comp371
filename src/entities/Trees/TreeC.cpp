#ifndef COMP371_TREE_C_cPP
#define COMP371_TREE_C_cPP

#include "TreeC.hpp"

    int TreeC::maxWidth(const float& trunkDiameter){
        return (int)(pow(spacingConstant * (trunkDiameter+1), 1.0/2) + trunkDiameter);
    }

    TreeC::TreeC(int numberOfTrees, const GLuint& shader_program,
                 Entity* entity, float trunkDiameter, long seed):
            Tree(heightChunking, boostFactor, shader_program, entity, 'C'){
        double duration;
        std::clock_t startTime;
        startTime = std::clock();

        //center piece
        auto * tci = new TreeClusterItem(shader_program, this, trunkDiameter, seed);
        tci->setLocationWithPoints(0, 0);
        tci->setPosition(glm::vec3(tci->xPos, 0, tci->zPos));
        treeCluster.push_back(tci);
        numberOfTrees--;

        //distribute in random cirlce
        float tempTrunkDiameter = trunkDiameter;
        int i = 0;
        for (; i < numberOfTrees; i++) {
            tempTrunkDiameter = TreeRandom::middleSquareRange(seed, sizeVariation*100, 0) / 100.0f;
            seed = (unsigned long)(pow(seed, 2) * (tempTrunkDiameter+1)) % 99991;

            float circleAngle = TreeRandom::middleSquareRange(seed, 360.0, 0.0);
            float distanceFromCenter = TreeRandom::middleSquareRange(seed, maxWidth(trunkDiameter), 1.0);

            float distScale = 30.0f;
            float xPos = cos(glm::radians(circleAngle)) * distanceFromCenter * distScale;
            float zPos =  sin(glm::radians(circleAngle)) * distanceFromCenter * distScale;

            auto * tci = new TreeClusterItem(shader_program, this,
                                                       tempTrunkDiameter <= 0? 0:  tempTrunkDiameter,
                                                       seed);

            tci->setLocationWithPoints(xPos , zPos);
            tci->setPosition(glm::vec3(tci->xPos, 0, tci->zPos));

            treeCluster.push_back(tci);
        }

        duration = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
        //printf("Duration of C %f Units: %f ms _CLUSTER HEAD\n", trunkDiameter, duration*1000);
    }


    std::vector<TreeClusterItem*> TreeC::getTreeCluster(){
        return treeCluster;
    }

    void TreeC::setSizeVariation(const float& r){
        sizeVariation = r;
    }

    TreeC::~TreeC(){
        for(TreeClusterItem* tci: treeCluster){
            delete tci;
        }
    }
#endif
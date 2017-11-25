//
// Created by Scherezade on 24/11/2017.
//

#include "Trunk.hpp"

void Trunk::computeUVs(int end , int start, int trunkPoints, float textureTrunkHeight, std::vector<glm::vec2>* trunkUVs){
        //Triangle function
        int top = (end - start) / trunkPoints;
        int circle = trunkPoints;
        for (GLuint i = 0 ; i < circle; i++){
            float v = std::abs((i* 100 + (int)(textureTrunkHeight* 100)) % (int)(textureTrunkHeight* 2 * 100) - textureTrunkHeight* 100) / 100;
            for (GLuint j = 0; j < top + 1; j++){
                float u = std::abs((j* 100 + (int)(textureTrunkHeight* 100) % (int)(textureTrunkHeight * 2 * 100) - textureTrunkHeight* 100)) / 100;
                trunkUVs->at(start + (i) * (circle) + j) = {u , v};
            }
        }
};

void Trunk::computeUVsConnectors(int start, int trunkPoints, float textureTrunkHeight, std::vector<glm::vec2>* trunkUVs){
    //Connector UV to top segment
    int top = 1;
    int circle = trunkPoints;
    for (GLuint i = 0 ; i <circle; i++){
        float v = std::abs((i* 100 + (int)(textureTrunkHeight* 100)) % (int)(textureTrunkHeight* 2 * 100) - textureTrunkHeight* 100) / 100;
        for (GLuint j = 0; j <  top; j++){
            float u = std::abs((j* 100 + (int)(textureTrunkHeight* 100) % (int)(textureTrunkHeight * 2 * 100) - textureTrunkHeight* 100)) / 100;
            trunkUVs->at(start + (i) * (circle) + j) = {u , v};
        }
    }
}
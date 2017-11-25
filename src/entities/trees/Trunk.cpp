//
// Created by Scherezade on 24/11/2017.
//

#include "Trunk.hpp"

void Trunk::computeUVs(AttatchmentGroupings* ag, int trunkPoints, float textureTrunkHeight, std::vector<glm::vec2>* trunkUVs){
        //Triangle function
    //std::cout << ag->end <<" "<< ag->start <<" "<< trunkPoints  <<" "<< textureTrunkHeight <<" "<< trunkUVs->size()<< "\n";
        int top = (ag->end  - ag->start) / trunkPoints;
        int circle = trunkPoints;
        for (GLuint i = 0 ; i < top; i++){
            float v = std::abs((i* 100 + (int)(textureTrunkHeight* 100)) % (int)(textureTrunkHeight* 2 * 100) - textureTrunkHeight* 100) / 100;
            //std::cout << v << "\n";
            trunkUVs->at( 1+ag->start + 0 + i * circle) = {0, v + ag->vHeight};
            trunkUVs->at( 1+ag->start + 1 + i * circle) = {1, v + ag->vHeight};
            trunkUVs->at( 1+ag->start + 2 + i * circle) = {0, v + ag->vHeight};
            trunkUVs->at( 1+ag->start + 2 + i * circle) = {0, v + ag->vHeight};
            trunkUVs->at( 1+ag->start + 3 + i * circle) = {1, v + ag->vHeight};
            trunkUVs->at( 1+ag->start + 4 + i * circle) = {0, v + ag->vHeight};
        }
};

void Trunk::computeUVsConnectors(AttatchmentGroupings* ag, int trunkPoints, float boostLength, float textureTrunkHeight, std::vector<glm::vec2>* trunkUVs){
    //Connector UV to top segment
//    int top = 1;
//    int circle = trunkPoints;
//    for (GLuint i = 0 ; i <circle; i++){
//        float v = std::abs((i* 100 + (int)(textureTrunkHeight* 100)) % (int)(textureTrunkHeight* 2 * 100) - textureTrunkHeight* 100) / 100;
//        for (GLuint j = 0; j <  top; j++){
//            float u = std::abs((j* 100 + (int)(textureTrunkHeight* 100) % (int)(textureTrunkHeight * 2 * 100) - textureTrunkHeight* 100)) / 100;
//            trunkUVs->at( ag->start + (i) * (circle) + j) = {u , v};
//        }
//    }
}
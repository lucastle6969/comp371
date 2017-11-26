/*
 *     ^
 *  <-[]->
 *    [x]
 * <--[-]-->
 *    [x]
 * <--[-]-->
 *
 * Goes up alternating leaf angle ending with a top leaf
 * Double sided index on leaves
 * Sets index after. Read comments carefully. Not intuitive.
 */

#include <cmath>
#include <cstdio>
#include <iostream>

#include "LeafContainerC.hpp"


LeafContainerC::LeafContainerC(std::vector<glm::vec3>* combinedVertices, std::vector<glm::vec2>* combinedUV, std::vector<glm::vec3>* combinedNormals, std::vector<GLuint>* combinedIndices,
                               float lineMax, float leafTextureHeight) {
    this->combinedVertices = combinedVertices;
    this->combinedNormals = combinedNormals;
    this->combinedUV = combinedUV;
    this->leafTextureHeight = leafTextureHeight;
    this->combinedIndices = combinedIndices;
    this->lineMax = lineMax;
}

void LeafContainerC::buildAllComponenets(const float& leafDiameter,const float& widthCutoff, const float& seed, float lineHeight){
    int randomSeedValue = TreeRandom::treeRandom(leafDiameter, seed, lineHeight);
    bool loopInitialTrunk = true;
    float lineSegments = leafDiameter * widthCutoff * stretchFactor;
    int baseVerticesSize = combinedVertices->size();
    int heightCount = 0;
    float itterations = 360.0f / leafPoints;
    float f = 1;

    float r2 = 0.000176135f * sqrt(std::abs(-88063572 + 50843527 * leafDiameter * lineSegments)) * f;
    float r1 = sqrt(3) * sqrt(std::abs(2 - r2 * r2)) * f;
    r1 = floor(r1) == 0 ? 1:r1;
    if(leafDiameter >=  widthCutoff/2) {
        while (lineHeight < lineMax - 2) {
            for (int n = 0; n < leafPoints; n++) {
                int sign = -1;
                int jagednessRandom = randomSeedValue * (((int) (n * 13.4) % 17) + 1);
                if (jagednessRandom % 2 == 0) sign *= -1;
                float tempTrunkDiameter = leafDiameter + sign * (jagednessRandom) % ((int) (ceil(leafDiameter))) *
                                                         jagednessFactor_Leaf / (leafDiameter);
                combinedVertices->push_back(
                        glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations * n)), lineHeight + 0,
                                  tempTrunkDiameter * cos(glm::radians(itterations * n))));
                unsigned  long s = combinedVertices->size();
                combinedUV->resize(s);
                combinedUV->at(s - 1) = {(n) % 2, 1 - leafTextureHeight * (heightCount % 2)};
            }
            //place leaves parralel following angle
            //place approproate leaves parralel following angle
            for (int n = 0; n < leafPoints; n++) {
                int sign = -1;
                int jagednessRandom = randomSeedValue * (((int) (n * 13.4) % 17) + 1);
                if (jagednessRandom % 2 == 0) sign *= -1;
                float tempTrunkDiameter =
                        r1 + sign * (jagednessRandom) % ((int) (ceil(r1))) * jagednessFactor_Leaf / leafDiameter;
                if (heightCount % 2 == 0)
                    combinedVertices->push_back(
                            glm::vec3(r2 * cos(glm::radians(itterations * n)) + tempTrunkDiameter / 2.0 + r2,
                                      lineHeight + r1 * sin(glm::radians(itterations * n)),
                                      0.0f));
                else
                    combinedVertices->push_back(glm::vec3(0,
                                                          lineHeight + r1 * sin(glm::radians(itterations * n)) + 0,
                                                          r2 * cos(glm::radians(itterations * n)) +
                                                          tempTrunkDiameter / 2.0 + r2));
            }
            setLeafUV();
            for (int n = 0; n < leafPoints; n++) {
                int sign = -1;
                int jagednessRandom = randomSeedValue * (((int) (n * 13.4) % 17) + 1);
                if (jagednessRandom % 2 == 0) sign *= -1;
                float tempTrunkDiameter =
                        r1 + sign * (jagednessRandom) % ((int) (ceil(r1))) * jagednessFactor_Leaf / leafDiameter;
                if (heightCount % 2 == 0)
                    combinedVertices->push_back(
                            glm::vec3(-r2 * cos(glm::radians(itterations * n)) - tempTrunkDiameter / 2.0 - r2,
                                      lineHeight + r1 * sin(glm::radians(itterations * n)), 0.0f));
                else
                    combinedVertices->push_back(glm::vec3(0,
                                                          lineHeight + r1 * sin(glm::radians(itterations * n)) + 0,
                                                          -r2 * cos(glm::radians(itterations * n)) -
                                                          tempTrunkDiameter / 2.0 - r2));
            }
            setLeafUV();
            lineHeight += lineSegments;
            heightCount++;
            //REPEAT NX
        }

        for (float y = 0.5; y >= 0.25; y -= 0.25) {
            f -= y * 0.55;
            //Generate circle
            unsigned  long s = 0;
            for (int n = 0; n < leafPoints; n++) {
                int sign = -1;
                int jagednessRandom = randomSeedValue * (((int) (n * 13.4) % 17) + 1);
                if (jagednessRandom % 2 == 0) sign *= -1;
                float tempTrunkDiameter = leafDiameter * y +
                                          sign * (jagednessRandom) % ((int) (ceil(leafDiameter * y))) *
                                          jagednessFactor_Leaf / (leafDiameter * y);
                combinedVertices->push_back(
                        glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations * n)), lineHeight + 0,
                                  tempTrunkDiameter * cos(glm::radians(itterations * n))));
                s = combinedVertices->size();
                combinedUV->resize(s);
                combinedUV->at(s - 1) = {(n) % 2, 1 - leafTextureHeight * (heightCount % 2)};
            }
            //place approproate leaves parralel following angle
            for (int n = 0; n < leafPoints; n++) {
                int sign = -1;
                int jagednessRandom = randomSeedValue * (((int) (n * 13.4) % 17) + 1);
                if (jagednessRandom % 2 == 0) sign *= -1;
                float tempTrunkDiameter =
                        r1 + sign * (jagednessRandom) % ((int) (ceil(r1))) * jagednessFactor_Leaf / (leafDiameter * y);
                if (heightCount % 2 == 0)
                    combinedVertices->push_back(
                            glm::vec3(r2 * cos(glm::radians(itterations * n)) + tempTrunkDiameter / 2.0 + r2,
                                      lineHeight + r1 * sin(glm::radians(itterations * n)), 0.0f));
                else
                    combinedVertices->push_back(glm::vec3(0, lineHeight + r1 * sin(glm::radians(itterations * n)) + 0,
                                                          r2 * cos(glm::radians(itterations * n)) +
                                                          tempTrunkDiameter / 2.0 + r2));
            }
            setLeafUV();
            for (int n = 0; n < leafPoints; n++) {
                int sign = -1;
                int jagednessRandom = randomSeedValue * (((int) (n * 13.4) % 17) + 1);
                if (jagednessRandom % 2 == 0) sign *= -1;
                float tempTrunkDiameter =
                        r1 + sign * (jagednessRandom) % ((int) (ceil(r1))) * jagednessFactor_Leaf / (leafDiameter * y);
                if (heightCount % 2 == 0)
                    combinedVertices->push_back(
                            glm::vec3(-r2 * cos(glm::radians(itterations * n)) - tempTrunkDiameter / 2.0 - r2,
                                      lineHeight + r1 * sin(glm::radians(itterations * n)), 0.0f));
                else
                    combinedVertices->push_back(glm::vec3(0, lineHeight + r1 * sin(glm::radians(itterations * n)) + 0,
                                                          -r2 * cos(glm::radians(itterations * n)) -
                                                          tempTrunkDiameter / 2.0 - r2));
            }
            setLeafUV();

            //place approproate leaves parralel following angle
            lineHeight += lineSegments * y * 0.8;
            heightCount++;
            //REPEAT 2X
        }
        
        int current = 0;
        for (; current < heightCount - 1/*don't do top*/; current++) {
            //branch
            int base = current * leafPoints * 3 + baseVerticesSize;
            int offset = (current + 1) * leafPoints * 3 + baseVerticesSize;
            //top and bottom
            for (int n = 0; n < leafPoints; n++) {
                combinedIndices->push_back(base + n);
                combinedIndices->push_back(base + (n + 1) % leafPoints);
                combinedIndices->push_back(offset + (n + 1) % leafPoints);
                combinedIndices->push_back(offset + (n + 1) % leafPoints);
                combinedIndices->push_back(offset + n);
                combinedIndices->push_back(base + n);
                
                unsigned  long s = combinedVertices->size();
                combinedNormals->resize(s);
                combinedNormals->at((base + n)) = glm::cross(
                        combinedVertices->at(offset + (n) % leafPoints) - combinedVertices->at(base + n),
                        combinedVertices->at(offset + n) - combinedVertices->at((base + n) )
                );
            }

            //leaf1 & 2 N and S side
            for (int n = 1; n <= 2; n++) {
                int base = current * leafPoints * 3 + baseVerticesSize + leafPoints * n;
//            north
                glm::vec3 leafNormal = glm::cross(
                        combinedVertices->at(base + 0) - combinedVertices->at(base + 1),
                        combinedVertices->at(base + 2) - combinedVertices->at((base + 1))
                );
                combinedIndices->push_back(base + 0);
                combinedIndices->push_back(base + 1);
                combinedIndices->push_back(base + 2);
                combinedIndices->push_back(base + 2);
                combinedIndices->push_back(base + 3);
                combinedIndices->push_back(base + 0);

                unsigned  long s = combinedVertices->size();
                combinedNormals->resize(s);
                combinedNormals->at((base + 0)) = leafNormal;
                combinedNormals->at((base + 1)) = -leafNormal;
                combinedNormals->at((base + 2)) = leafNormal;
                combinedNormals->at((base + 3)) = -leafNormal;
                combinedNormals->at((base + 4)) = leafNormal;

//            south
                combinedIndices->push_back(base + 0);
                combinedIndices->push_back(base + 3);
                combinedIndices->push_back(base + 2);
                combinedIndices->push_back(base + 2);
                combinedIndices->push_back(base + 1);
                combinedIndices->push_back(base + 0);
            }
        }
        //leaf1 & 2 N and S side on the top(ie from the don't do top)
        for (int n = 1; n <= 2; n++) {
            int base =  (heightCount - 1) * leafPoints * 3 + baseVerticesSize + leafPoints * n;
            glm::vec3 leafNormal = glm::cross(
                    combinedVertices->at(base + 0) - combinedVertices->at(base + 1),
                    combinedVertices->at(base + 2) - combinedVertices->at((base + 1))
            );
            //north
            combinedIndices->push_back(base + 0);
            combinedIndices->push_back(base + 1);
            combinedIndices->push_back(base + 2);
            combinedIndices->push_back(base + 2);
            combinedIndices->push_back(base + 3);
            combinedIndices->push_back(base + 0);

            unsigned  long s = combinedVertices->size();
            combinedNormals->resize(s);
            combinedNormals->at((base + 0)) = leafNormal;
            combinedNormals->at((base + 1)) = -leafNormal;
            combinedNormals->at((base + 2)) = leafNormal;
            combinedNormals->at((base + 3)) = -leafNormal;

            //south
            combinedIndices->push_back(base + 0);
            combinedIndices->push_back(base + 3);
            combinedIndices->push_back(base + 2);
            combinedIndices->push_back(base + 2);
            combinedIndices->push_back(base + 1);
            combinedIndices->push_back(base + 0);
        }
    }

    //Cap with a leaf
    for (int n = 0; n < leafPoints ; n++) {
        int sign = -1;
        int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
        if (jagednessRandom % 2 == 0) sign *= -1;
        float tempTrunkDiameter = leafDiameter + sign * (jagednessRandom) % ((int)(ceil(leafDiameter))) * jagednessFactor_Leaf / (leafDiameter);
        combinedVertices->push_back(glm::vec3(r1 * sin(glm::radians(itterations  * n)) , r2 *  cos(glm::radians(itterations  * n)) + lineHeight + r2, 0 ));
    }
    //fill it
    setLeafUV();
    int base = combinedVertices->size() - leafPoints;
//    north
    combinedIndices->push_back(base+0); combinedIndices->push_back(base + 1); combinedIndices->push_back(base + 2);
    combinedIndices->push_back(base + 2);  combinedIndices->push_back(base + 3); combinedIndices->push_back(base+0);
    //south
    combinedIndices->push_back(base + 0);  combinedIndices->push_back(base + 3); combinedIndices->push_back(base + 2);
    combinedIndices->push_back(base+2); combinedIndices->push_back(base + 1); combinedIndices->push_back(base + 0);

    glm::vec3 leafNormal = glm::cross(
            combinedVertices->at(base + 0) - combinedVertices->at(base + 1),
            combinedVertices->at(base + 2) - combinedVertices->at((base + 1))
    );
    unsigned  long s = combinedVertices->size();
    combinedNormals->resize(s);
    combinedNormals->at((base + 0)) = leafNormal;
    combinedNormals->at((base + 1)) = -leafNormal;
    combinedNormals->at((base + 2)) = leafNormal;
    combinedNormals->at((base + 3)) = -leafNormal;
}

void LeafContainerC::setLeafUV(){
    unsigned  long s = combinedVertices->size();
    combinedUV->resize(s);
    combinedUV->at(s - 1 - 3) = {1.0, 0 + (1 - leafTextureHeight) * 0.5f};
    combinedUV->at(s - 1 - 2) = {0.5, 1 - leafTextureHeight};
    combinedUV->at(s - 1 - 1) = {0,  (1 - leafTextureHeight) * 0.5f};
    combinedUV->at(s - 1 - 0) = {0.5, 0};
}

#ifndef PROCEDURALWORLD_TRUNKC_C
#define PROCEDURALWORLD_TRUNKC_C


#include <iostream>
#include "TrunkC.hpp"

TrunkC::TrunkC(std::vector<glm::vec3>* combinedVertices, std::vector<glm::vec2>* combinedUV, std::vector<glm::vec3>* combinedNormals,
               std::vector<GLuint>* combinedIndices,
               float lineMax, float textureHeight){
    this->combinedVertices = combinedVertices;
    this->combinedNormals = combinedNormals;
    this->combinedUV = combinedUV;
    this->textureHeight = textureHeight;
    this->combinedIndices = combinedIndices;
    this->lineMax = lineMax;
}

float TrunkC::buildAllComponents(const float& trunkDiameter, const float& seed, float lineHeight){
    int randomSeedValue = TreeRandom::treeRandom(trunkDiameter, seed, lineHeight);
    bool loopInitialTrunk = true;
    const float lineSegments = lineMax / heightChunking;
    const int baseVerticesSize = combinedVertices->size();
    int count = 0;
    const float itterations = 360.0f / trunkPoints;
    //MOVE PROCEEDURE TO TRUNK CLASS
    do {
        //build points
        for (int y = 0; y < 3; y++) {
            for (int n = 0; n < trunkPoints; n++) {
                int sign = -1;
                const int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1 + y);
                if (jagednessRandom % 2 == 0) sign *= -1;
                float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter))) * jagednessFactor / (trunkDiameter);
                glm::vec3 circleEdge(
                        tempTrunkDiameter * sin(glm::radians(itterations  * n)) ,
                        lineHeight + 0,
                        tempTrunkDiameter *  cos(glm::radians(itterations  * n))
                );
                combinedVertices->push_back(circleEdge);
                unsigned  long s = combinedVertices->size();
                combinedUV->resize(s);
                combinedUV->at(s - 1) = {(n) % 2, 1 - textureHeight * (y % 2)};
            }
            lineHeight += lineSegments;
            randomSeedValue = TreeRandom::treeRandom(trunkDiameter, seed, lineHeight);
        }
        //build indices
        for (int y = 0; y < 2; y++) {
            for (int n = 0; n < trunkPoints; n++) {
                const int offset = (y + 1) * trunkPoints + baseVerticesSize;
                const int base = y * trunkPoints + baseVerticesSize;
                //WORKS
                combinedIndices->push_back(base + n + count * trunkPoints);
                combinedIndices->push_back(base + (n + 1) % trunkPoints + count * trunkPoints);
                combinedIndices->push_back(offset +(n + 1)% trunkPoints + count * trunkPoints);
                combinedIndices->push_back(offset + (n + 1)% trunkPoints + count * trunkPoints);
                combinedIndices->push_back(offset + n + count * trunkPoints);
                combinedIndices->push_back(base + n + count * trunkPoints);

                unsigned  long s = combinedVertices->size();
                combinedNormals->resize(s);
                combinedNormals->at(base + n + count * trunkPoints) = glm::cross(
                        combinedVertices->at(base + (n + 1) %  trunkPoints + count * trunkPoints) - combinedVertices->at(base + n %   trunkPoints+ count * trunkPoints),
                        combinedVertices->at(offset + n + count * trunkPoints) - combinedVertices->at(base + n %  trunkPoints + count * trunkPoints)
                );
                if (count > 0 && y == 0) {
                    int countTemp = count - 1;
                    combinedIndices->push_back(base + n + countTemp * trunkPoints);
                    combinedIndices->push_back((base + (n + 1) % trunkPoints  + countTemp * trunkPoints));
                    combinedIndices->push_back((offset +( n + 1 ) % trunkPoints+ countTemp * trunkPoints));
                    combinedIndices->push_back((offset + (n + 1) % trunkPoints + countTemp * trunkPoints));
                    combinedIndices->push_back(offset + n + countTemp * trunkPoints);
                    combinedIndices->push_back(base + n + countTemp * trunkPoints);
                    combinedNormals->at(base + n  + countTemp * trunkPoints) = glm::cross(
                            combinedVertices->at(base + (n + 1) % trunkPoints + countTemp  * trunkPoints) - combinedVertices->at((base + n)  + countTemp * trunkPoints),
                            combinedVertices->at((offset + n) + countTemp * trunkPoints) - combinedVertices->at((base + n)  + countTemp * trunkPoints)
                    );
                }
            }
        }
        count += 3;

        if ((randomSeedValue * (count + 1)) % branchMod == 0) {
            loopInitialTrunk = false;
        }
    } while (loopInitialTrunk && lineHeight < lineMax);
    if (lineHeight >= lineMax)
        return -1;
    else if(!loopInitialTrunk)
        return lineHeight;
    else return -1;
}

#endif //PROCEDURALWORLD_TRUNKC_H


#ifndef PROCEDURALWORLD_TRUNKC_C
#define PROCEDURALWORLD_TRUNKC_C


#include "TrunkC.hpp"

    TrunkC::TrunkC(std::vector<glm::vec3>* combinedVertices, std::vector<GLuint>* combinedIndices,
           float lineMax){
        this->combinedVertices = combinedVertices;
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

                }
                lineHeight += lineSegments;
                randomSeedValue = TreeRandom::treeRandom(trunkDiameter, seed, lineHeight);
            }
            //build indices
            for (int y = 0; y < 2; y++) {
                for (int n = 0; n < trunkPoints; n++) {
                    const int offset = (y + 1) * trunkPoints + baseVerticesSize;
                    const int base = y * trunkPoints + baseVerticesSize;
                    if (n == trunkPoints - 1) {
                        combinedIndices->push_back(base + n + count * trunkPoints);
                        combinedIndices->push_back(base + count * trunkPoints);
                        combinedIndices->push_back(offset + count * trunkPoints);
                        combinedIndices->push_back(offset + count * trunkPoints);
                        combinedIndices->push_back(offset + n + count * trunkPoints);
                        combinedIndices->push_back(base + n + count * trunkPoints);
                    }
                    else {
                        //WORKS
                        combinedIndices->push_back(base + n + count * trunkPoints);
                        combinedIndices->push_back(base + n + 1 + count * trunkPoints);
                        combinedIndices->push_back(offset + n + 1 + count * trunkPoints);
                        combinedIndices->push_back(offset + n + 1 + count * trunkPoints);
                        combinedIndices->push_back(offset + n + count * trunkPoints);
                        combinedIndices->push_back(base + n + count * trunkPoints);
                    }
                    if (count > 0 && y == 0) {
                        int countTemp = count - 1;
                        if (n == trunkPoints - 1) {
                            combinedIndices->push_back(base + n + countTemp * trunkPoints);
                            combinedIndices->push_back(base + countTemp * trunkPoints);
                            combinedIndices->push_back(offset + countTemp * trunkPoints);
                            combinedIndices->push_back(offset + countTemp * trunkPoints);
                            combinedIndices->push_back(offset + n + countTemp * trunkPoints);
                            combinedIndices->push_back(base + n + countTemp * trunkPoints);
                        }
                        else {
                            combinedIndices->push_back(base + n + countTemp * trunkPoints);
                            combinedIndices->push_back(base + n + 1 + countTemp * trunkPoints);
                            combinedIndices->push_back(offset + n + 1 + countTemp * trunkPoints);
                            combinedIndices->push_back(offset + n + 1 + countTemp * trunkPoints);
                            combinedIndices->push_back(offset + n + countTemp * trunkPoints);
                            combinedIndices->push_back(base + n + countTemp * trunkPoints);
                        }
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

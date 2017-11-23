
#include "LeafContainerC.hpp"


LeafContainerC::LeafContainerC(std::vector<glm::vec3>* combinedVertices, std::vector<GLuint>* combinedIndices,
float lineMax) {
    this->combinedVertices = combinedVertices;
    this->combinedIndices = combinedIndices;
    this->lineMax = lineMax;
}

void LeafContainerC::buildAllComponenets(const float& leafDiameter, const float& seed, float lineHeight){
    int randomSeedValue = TreeRandom::treeRandom(leafDiameter, seed, lineHeight);
    bool loopInitialTrunk = true;
    float lineSegments = leafDiameter * 3.0f;
    int baseVerticesSize = combinedVertices->size();
    int count = 0;
    float itterations = 360.0f / leafPoints;
    float f = 1;

    while (lineMax % 3 != 0) lineMax--;

    float r2 = 0.000176135f * sqrt(abs(-88063572 + 50843527 * leafDiameter * lineSegments)) * f;
    float r1 = sqrt(3) * sqrt(abs(2 - r2 * r2)) * f;
    r1 = floor(r1) == 0 ? 1:r1;

    while (lineHeight < lineMax - 2){
        for (int n = 0; n < leafPoints ; n++) {
            int sign = -1;
            int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
            if (jagednessRandom % 2 == 0) sign *= -1;
            float tempTrunkDiameter = leafDiameter + sign * (jagednessRandom) % ((int)(ceil(leafDiameter))) * jagednessFactor_Leaf / (leafDiameter);
            combinedVertices->push_back(glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations  * n)) , lineHeight + 0, tempTrunkDiameter *  cos(glm::radians(itterations  * n)) ));
        }
        //place leaves parralel following angle
        //place approproate leaves parralel following angle
        for (int n = 0; n < leafPoints; n++) {
            int sign = -1;
            int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
            if (jagednessRandom % 2 == 0) sign *= -1;
            float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / leafDiameter;
            if (count % 2 == 0)
                combinedVertices->push_back(glm::vec3(r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , 0.0f));
            else
                combinedVertices->push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 ));

        }
        for (int n = 0; n < leafPoints; n++) {
            int sign = -1;
            int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
            if (jagednessRandom % 2 == 0) sign *= -1;
            float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / leafDiameter;
            if (count % 2 == 0)
                combinedVertices->push_back(glm::vec3(-r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , 0.0f));
            else
                combinedVertices->push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, -r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 ));

        }
        lineHeight += lineSegments;
        count++;
        //REPEAT NX
    }

    for (float y = 0.5; y >= 0.25; y -= 0.25) {
        f -= y * 0.55;
        //Generate circle
        for (int n = 0; n < leafPoints; n++) {
            int sign = -1;
            int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
            if (jagednessRandom % 2 == 0) sign *= -1;
            float tempTrunkDiameter = leafDiameter * y + sign * (jagednessRandom) % ((int)(ceil(leafDiameter* y))) * jagednessFactor_Leaf / (leafDiameter* y);
            combinedVertices->push_back(glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations  * n)) , lineHeight + 0, tempTrunkDiameter *  cos(glm::radians(itterations  * n)) ));

        }

        //place approproate leaves parralel following angle
        for (int n = 0; n < leafPoints; n++) {
            int sign = -1;
            int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
            if (jagednessRandom % 2 == 0) sign *= -1;
            float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / (leafDiameter* y);
            if (count % 2 == 0)
                combinedVertices->push_back(glm::vec3(r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , 0.0f));
            else
                combinedVertices->push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 ));

        }
        for (int n = 0; n < leafPoints; n++) {
            int sign = -1;
            int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
            if (jagednessRandom % 2 == 0) sign *= -1;
            float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / (leafDiameter* y);
            if (count % 2 == 0)
                combinedVertices->push_back(glm::vec3(-r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , 0.0f));
            else
                combinedVertices->push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, -r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 ));

        }
        lineHeight += lineSegments * y * 0.8;
        count++;
        //REPEAT 2X
    }
    int len = (float)(lineMax - 2) / lineSegments + 3;
    for (int i = 0; i < len; i++) {
        //branch
        int base = i * leafPoints * 3 + baseVerticesSize;
        int offset = (i + 1)*leafPoints * 3 + baseVerticesSize;
        if (i != len - 1) {
            for (int n = 0; n < leafPoints; n++) {
                if (n == leafPoints - 1) {
                    combinedIndices->push_back(base + n); combinedIndices->push_back(base + 0); combinedIndices->push_back(offset + 0);
                    combinedIndices->push_back(offset + 0);  combinedIndices->push_back(offset + n); combinedIndices->push_back(base + n);
                }
                else {
                    combinedIndices->push_back(base + n); combinedIndices->push_back(base + n + 1); combinedIndices->push_back(offset + n + 1);
                    combinedIndices->push_back(offset + n + 1);  combinedIndices->push_back(offset + n); combinedIndices->push_back(base + n);
                }
            }
        }
        else {
            for (int n = 0; n < leafPoints - 2; n++) {
                int base = i * leafPoints * 3 + baseVerticesSize;
                if (n == leafPoints - 2 - 1) {
                    combinedIndices->push_back(base+0); combinedIndices->push_back(base + 1); combinedIndices->push_back(base + 2);
                    combinedIndices->push_back(base + 2);  combinedIndices->push_back(base - n); combinedIndices->push_back(base+0);
                }
                else {
                    combinedIndices->push_back(base+0); combinedIndices->push_back(base + 1); combinedIndices->push_back(base + 2);
                    combinedIndices->push_back(base + 2);  combinedIndices->push_back(base + 3); combinedIndices->push_back(base+0);
                }
                if (n == leafPoints - 2 - 1) {
                    combinedIndices->push_back(base+0); combinedIndices->push_back(base - n); combinedIndices->push_back(base + 2);
                    combinedIndices->push_back(base + 2); combinedIndices->push_back(base + 1); combinedIndices->push_back(base+0);
                }
                    //topsegment cover
                else {
                    combinedIndices->push_back(base+0); combinedIndices->push_back(base + 3); combinedIndices->push_back(base + 2);
                }
            }
        }
        //leaf1&2 N and S side
        for (int j = 0; j < 2; j++) {
            for (int n = 0; n < leafPoints - 2; n++) {
                int base = leafPoints * (j + 1) + i * leafPoints * 3 + baseVerticesSize + n;
                //NORTH
                if (n == leafPoints - 2 - 1) {
                    combinedIndices->push_back(base+0); combinedIndices->push_back(base + 1); combinedIndices->push_back(base + 2);
                    combinedIndices->push_back(base + 2);  combinedIndices->push_back(base - n); combinedIndices->push_back(base+0);
                }
                else {
                    combinedIndices->push_back(base+0); combinedIndices->push_back(base+0 + 1); combinedIndices->push_back(base + 2);
                    combinedIndices->push_back(base + 2);  combinedIndices->push_back(base + 3); combinedIndices->push_back(base+0);
                }
                //SOUTH
                if (n == leafPoints - 2 - 1) {
                    combinedIndices->push_back(base+0); combinedIndices->push_back(base - n); combinedIndices->push_back(base + 2);
                    combinedIndices->push_back(base + 2); combinedIndices->push_back(base + 1); combinedIndices->push_back(base+0);
                }
                else {
                    combinedIndices->push_back(base+0); combinedIndices->push_back(base + 3); combinedIndices->push_back(base + 2);
                    combinedIndices->push_back(base + 2); combinedIndices->push_back(base + 1); combinedIndices->push_back(base+0);
                }
            }
        }
    }

    //Cap with a leaf
    //r2 = 0.000176135 * sqrt(abs(-88063572 + 50843527 * leafDiameter * lineSegments))* f;
    //r1 = sqrt(3) * sqrt(abs(2 - r2 * r2)) * f;
    for (int n = 0; n < leafPoints ; n++) {
        int sign = -1;
        int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
        if (jagednessRandom % 2 == 0) sign *= -1;
        float tempTrunkDiameter = leafDiameter + sign * (jagednessRandom) % ((int)(ceil(leafDiameter))) * jagednessFactor_Leaf / (leafDiameter);
        combinedVertices->push_back(glm::vec3(r1 * sin(glm::radians(itterations  * n)) , r2 *  cos(glm::radians(itterations  * n)) + lineHeight + r2, 0 ));

    }
    for (int n = 0; n < leafPoints - 2; n++) {
        int base = combinedVertices->size() - leafPoints + n;
        if (n == leafPoints - 2 - 1) {
            combinedIndices->push_back(base+0); combinedIndices->push_back(base + 1); combinedIndices->push_back(base + 2);
            combinedIndices->push_back(base + 2);  combinedIndices->push_back(base - n); combinedIndices->push_back(base+0);
        }
        else {
            combinedIndices->push_back(base+0); combinedIndices->push_back(base + 1); combinedIndices->push_back(base + 2);
            combinedIndices->push_back(base + 2);  combinedIndices->push_back(base + 3); combinedIndices->push_back(base+0);
        }
        if (n == leafPoints - 2 - 1) {
            combinedIndices->push_back(base+0); combinedIndices->push_back(base - n); combinedIndices->push_back(base + 2);
            combinedIndices->push_back(base + 2); combinedIndices->push_back(base + 1); combinedIndices->push_back(base+0);
        }
        else {
            combinedIndices->push_back(base+0); combinedIndices->push_back(base + 3); combinedIndices->push_back(base + 2);
            combinedIndices->push_back(base + 2); combinedIndices->push_back(base + 1); combinedIndices->push_back(base+0);
        }
    }
}

#include "LeafBranchA.hpp"

void LeafBranchA::buildLeafBranch(float trunkDiameter){
    for (int n = 0; n < branchPoints ; n++) {
        int sign = -1;
        int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
        if (jagednessRandom % 2 == 0) sign *= -1;
        float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter))) * jagednessFactor_Leaf / (trunkDiameter);
        leafVertices->push_back(glm::vec3(
                tempTrunkDiameter * sin(glm::radians(itterations  * n)) ,
                lineHeight,
                tempTrunkDiameter *  cos(glm::radians(itterations  * n))));
    }
}

void LeafBranchA::buildBranchElements(int baseVerticesSize, int i, int len){
    int base = i * branchPoints* 3 + baseVerticesSize;
    int set = (i + 1)*branchPoints* 3 + baseVerticesSize;
    int nSolution = 0;
    for (int n = 0; n < branchPoints; n++) {
        if (n == branchPoints- 1) {
            leafIndices->push_back(base + n);
            leafIndices->push_back(base + 0);
            leafIndices->push_back(set + 0);
            leafIndices->push_back(set + 0);
            leafIndices->push_back(set + n);
            leafIndices->push_back(base + n);
            ////printf("%f %f\n", (float)(n + nSolution) / branchPoints, (float)i / (len) );
            leafUVs->push_back(glm::vec2((float) (n + nSolution) / branchPoints, (float) i / (len)));
            nSolution++;
        }
        else {
            leafIndices->push_back(base + n);
            leafIndices->push_back(base + n + 1);
            leafIndices->push_back(set + n + 1);
            leafIndices->push_back(set + n + 1);
            leafIndices->push_back(set + n);
            leafIndices->push_back(base + n);
        }
        ////printf("%f %f\n", (float)(n + nSolution) / branchPoints, (float)i / (len) );
        leafUVs->push_back(glm::vec2((float)(n + nSolution) / branchPoints, (float)i / (len)));
    }
    for (int n = 0; n < branchPoints+ 1; n++) {
        ////printf("%f %f\n", (float)(n) / branchPoints, (float)(i+1) / (len) );
        leafUVs->push_back(glm::vec2((float)(n + nSolution) / branchPoints, (float)(i+1) / (len)));
    }
}
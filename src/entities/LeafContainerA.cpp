//
// Created by Tof on 2017-11-19.
//

#include "LeafContainerA.hpp"

void LeafContainerA::buildVertices(int randomSeedValue, float lineSegments, int lineMax, float trunkDiameter, float seed,
                   float lineHeight, int* baseVerticesSize){
    //MOVE PROCEDURE OF LEAF BRANCHES TO LEAFBRANCH CLASS AND LEAVES TO THE LEAF CLASS
    //CONDENSE ALGORITHM SIZE
    float f = 2;
    float branchItterations = 360.0f / leafBranchPoints;
    float leafItterations = 360.0f/ leafPoints;

    //consume the lineMax if it's not a multiple of three
    while (lineMax % 3 != 0) lineMax--;

    //elipse radius values
    float r2 = 0.000176135 * sqrt(abs(-88063572 + 50843527 * trunkDiameter * lineSegments)) ;
    float r1 = sqrt(3) * sqrt(abs(2 - r2 * r2)) * f;
    r1 = floor(r1) == 0 ? 1:r1;

    int count = 0;
    while (lineHeight < lineMax - 2){
        //branch
        LeafBranchA LBA(randomSeedValue, branchItterations, leafBranchPoints,
                        lineHeight, jagednessFactor_Leaf, leafVertices, nullptr, nullptr);
        LBA.buildLeafBranch(trunkDiameter);

        //builds 2 leaves at alternating 90 degrees. Uses count to do this.
        LeafA LA(leafPoints, randomSeedValue, count, lineHeight, branchItterations, jagednessFactor_Leaf,
                 leafVertices, nullptr, nullptr);
        LA.buildLeaf(r1, r2, r1, count);

        lineHeight += lineSegments;
        count++;
        //REPEAT NX
    }


    for (float y = 0.5; y >= 0.25; y -= 0.25) {
        f -= y * 0.55;

        //the branch is getting smaller
        LeafBranchA LBA(randomSeedValue, branchItterations, leafBranchPoints,
                        lineHeight, jagednessFactor_Leaf, leafVertices, nullptr, nullptr);
        LBA.buildLeafBranch(trunkDiameter*y);

        //still making leaves
        LeafA LA(leafPoints, randomSeedValue, count, lineHeight, leafItterations, jagednessFactor_Leaf,
                 leafVertices, nullptr, nullptr);
        LA.buildLeaf(r1, r2, r1, count);

        lineHeight += lineSegments * y * 0.8;
        count++;
        //REPEAT 2X
    }
    //Cap with a leaf
    LeafA LA(leafPoints, randomSeedValue, count, lineHeight, leafItterations, jagednessFactor_Leaf,
             leafVertices, leafIndices, leafUVs);
    LA.buildLeafSingle(r1, r2, r1);
}


void LeafContainerA::buildContainer(float trunkDiameter, float seed, float lineHeight, int lineMax){
    int randomSeedValue = TreeRandom::treeRandom(trunkDiameter, seed, lineHeight);

    float lineSegments = trunkDiameter * 3.0;//((float)lineMax) / heightChunking;
    int baseVerticesSize = leafVertices->size();

    //leaves are in altrenating 90 degrees and stord on a branch
    buildVertices(randomSeedValue, lineSegments, lineMax, trunkDiameter, seed, lineHeight, &baseVerticesSize);
}

void LeafContainerA::buildLeafContainerElements(int start, int end,
                                       std::vector<GLuint>* leafIndices, std::vector<glm::vec3>* leafVert,
                                       std::vector<glm::vec2>* leafUVs, std::vector<glm::vec3>* leafNorms){

    int endVert = (end - start - leafPoints*1)/ (leafBranchPoints + leafPoints*2);
    int itter = leafBranchPoints + leafPoints * 2;
    int vertToNext = ((1) *(leafBranchPoints + leafPoints*2));
    int factor = (leafBranchPoints + leafPoints*2);

    GLuint i = 0;
    for(int k = 0 ; k < endVert; k++){
        for(int j = i; i < leafBranchPoints + j ; i++) {
            int currentVertSet = ((k) *factor);
            GLuint i1 = (i + 1) % (factor) % leafBranchPoints +  currentVertSet;

            leafIndices->push_back(i + start);
            leafIndices->push_back(i1+ start);
            leafIndices->push_back(i1 + vertToNext + start);
            leafIndices->push_back(i1  +  vertToNext + start);
            leafIndices->push_back(i  +  vertToNext + start);
            leafIndices->push_back(i+ start);

            leafNorms->push_back(glm::cross(
                    leafVert->at(i1) - leafVert->at(i),
                    leafVert->at(i + leafBranchPoints+ start) - leafVert->at(i+ start)
            ));
        }

        glm::vec3 leafNormal = (glm::cross(
                leafVert->at(i + 1+ start) - leafVert->at(i + start),
                leafVert->at(i +2+ start) - leafVert->at(i + start)
        ));

        i = LeafA::buildElements(i, start, leafNormal, leafPoints , leafIndices, leafVert,
                                 leafUVs, leafNorms);
        i = LeafA::buildElements(i, start, leafNormal, leafPoints , leafIndices, leafVert,
                                 leafUVs, leafNorms);
    }

    //incides and normal of top leaf
    glm::vec3 leafNormal = (glm::cross(
            leafVert->at(i + leafPoints + start + 1) - leafVert->at(i + leafPoints + start),
            leafVert->at(i + leafPoints + start + 2) - leafVert->at(i + leafPoints + start)
    ));

    i = LeafA::buildElements(i, start, leafNormal, leafPoints , leafIndices, leafVert,
                             leafUVs, leafNorms);
    i = LeafA::buildElements(i, start, leafNormal, leafPoints , leafIndices, leafVert,
                             leafUVs, leafNorms);

    i = LeafA::buildElements(i, start, leafNormal, leafPoints , leafIndices, leafVert,
                             leafUVs, leafNorms);

    i = LeafA::buildElements(i, start, leafNormal, leafPoints , leafIndices, leafVert,
                             leafUVs, leafNorms);
}
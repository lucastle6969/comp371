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
    while (lineMax % 3 != 0) lineMax--;

    float r2 = 0.000176135 * sqrt(abs(-88063572 + 50843527 * trunkDiameter * lineSegments)) ;
    float r1 = sqrt(3) * sqrt(abs(2 - r2 * r2)) * f;

    int count = 0;
//    printf("2---%f < %f  ---- %f %f\n", lineHeight, lineMax - 2, lineSegments, trunkDiameter);
    while (lineHeight < lineMax - 2){
        //branch
        LeafBranchA LBA(randomSeedValue, branchItterations, leafBranchPoints,
                        lineHeight, jagednessFactor_Leaf, leafVertices, nullptr, nullptr);
        LBA.buildLeafBranch(trunkDiameter);

        //builds 2
        LeafA LA(leafPoints, randomSeedValue, count, lineHeight, branchItterations, jagednessFactor_Leaf,
                 leafVertices, nullptr, nullptr);
        LA.buildLeaf(r1, r2, r1);

        lineHeight += lineSegments;
        count++;
        //REPEAT NX
    }

    for (float y = 0.5; y >= 0.25; y -= 0.25) {
        f -= y * 0.55;

        LeafBranchA LBA(randomSeedValue, branchItterations, leafBranchPoints,
                        lineHeight, jagednessFactor_Leaf, leafVertices, nullptr, nullptr);
        LBA.buildLeafBranch(trunkDiameter*y);

        LeafA LA(leafPoints, randomSeedValue, count, lineHeight, leafItterations, jagednessFactor_Leaf,
                 leafVertices, nullptr, nullptr);
        LA.buildLeaf(r1, r2, r1);

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
    int count = 0;

    //printf("1----%f < %f  ---- %f %f\n", lineHeight, lineMax - 2, lineSegments, trunkDiameter);
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
        //build indices and normal of branch segment.
        //printf("--- %d %d %d\n", i , start, k);
        /* Your algorithm here */
        for(int j = i; i < leafBranchPoints + j ; i++) {
            int currentVertSet = ((k) *factor);
            GLuint i1 = (i + 1) % (factor) % leafBranchPoints +  currentVertSet;
            //printf("%i %i %i %i\n", vertToNext, currentVertSet, i, i1);
            //printf("%i %i %i %i\n", i + start, i1+ start, i1 +  vertToNext + start, i  +  vertToNext + start);
            leafIndices->push_back(i + start);
            leafIndices->push_back(i1+ start);
            leafIndices->push_back(i1 + vertToNext + start);
            leafIndices->push_back(i1  +  vertToNext + start);
            leafIndices->push_back(i  +  vertToNext + start);
            leafIndices->push_back(i+ start);
            //printf("%d %d \n", i , i1);
            leafNorms->push_back(glm::cross(
                    leafVert->at(i1) - leafVert->at(i),
                    leafVert->at(i + leafBranchPoints+ start) - leafVert->at(i+ start)
            ));
        }
//            float increments = ((end - start) / leafBranchPoints) / 2.0;
//            float   jumps = textureLeafStart / increments;
//            for (GLuint j = 0; j < end; j ++){
//                float v = abs(-textureLeafStart + jumps*j/leafBranchPoints);
//                leafUVs->push_back(glm::vec2(((float)j) / leafBranchPoints, v));
//            }



        //printf("2 %d %d %d\n", i , start, leafVert->size());
        glm::vec3 leafNormal = (glm::cross(
                leafVert->at(i + 1+ start) - leafVert->at(i + start),
                leafVert->at(i +2+ start) - leafVert->at(i + start)
        ));

        i = LeafA::buildElements(i, start, leafNormal, leafPoints , leafIndices, leafVert,
                                 leafUVs, leafNorms);
        i = LeafA::buildElements(i, start, leafNormal, leafPoints , leafIndices, leafVert,
                                 leafUVs, leafNorms);
    }

    //printf("4 %d %d %d\n", i , start, leafVert->size());
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

    //printf("%i i %i start %i end %i actual\n", i, start, end, i + start);
}
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
 * Sets index after. Read comments carefully.
 */

#include <cmath>

#include "LeafContainerAB.hpp"

constexpr int LeafContainerAB::leafPoints;
constexpr int LeafContainerAB::leafBranchPoints;

constexpr float LeafContainerAB::itterationsLeaf;
constexpr float LeafContainerAB::itterationsLeafBranch;

constexpr float LeafContainerAB::jagednessFactor_Leaf;
constexpr float LeafContainerAB::textureLeafStart;
constexpr float LeafContainerAB::textureLeafEnd;

LeafContainerAB::LeafContainerAB(std::vector<glm::vec3>* leafVertices,
                                 std::vector<GLuint>* leafIndices,
                                 std::vector<glm::vec2>* leafUVs,
                                 const int& seed){
    this->leafVertices = leafVertices;
    this->leafIndices = leafIndices;
    this->leafUVs = leafUVs;
    baseVerticesSize = leafVertices->size();
    this->seed = seed;
}

void LeafContainerAB::buildContainer(float trunkDiameter, const float& seed, float lineHeight, const int& lineMax){
    int randomSeedValue = TreeRandom::treeRandom(trunkDiameter, seed, lineHeight);

    const float lineSegments = trunkDiameter * 3.0;
    int baseVerticesSize = leafVertices->size();

    //leaves are in altrenating 90 degrees and stord on a branch
    buildVertices(randomSeedValue, lineSegments, lineMax, trunkDiameter, seed, lineHeight, &baseVerticesSize);
}

void LeafContainerAB::buildVertices(const int& randomSeedValue, float lineSegments,
                                    int lineMax, float trunkDiameter, const float& seed,
                                    float lineHeight, int* baseVerticesSize){
    float f = 2;
    const float branchItterations = 360.0f / leafBranchPoints;
    const float leafItterations = 360.0f/ leafPoints;

    //consume the lineMax if it's not a multiple of three
    while (lineMax % 3 != 0) lineMax--;

    //elipse radius values
    float r2 = 0.000176135f * sqrt(std::abs(-88063572 + 50843527 * trunkDiameter * lineSegments)) ;
    float r1 = sqrt(3) * sqrt(std::abs(2 - r2 * r2)) * f;
    r1 = floor(r1) == 0 ? 1:r1;

    int count = 0;
    while (lineHeight < lineMax - 2){
        //branch
        LeafBranchAB LBA(randomSeedValue, branchItterations, leafBranchPoints,
                         lineHeight, jagednessFactor_Leaf, leafVertices, nullptr,leafUVs);
        LBA.buildLeafBranch(trunkDiameter, count, textureLeafStart);

        //builds 2 leaves at alternating 90 degrees. Uses count to do this.
        LeafA LA(leafPoints, randomSeedValue, count, lineHeight, branchItterations, jagednessFactor_Leaf,
                 leafVertices, nullptr, leafUVs);
        //north and south
        std::cout << leafVertices->size() << " ";
        LA.buildLeaf(r1, r2, r1, count, textureLeafStart); //build two double sided leaves
        std::cout << leafVertices->size() << "==== \n ";
        lineHeight += lineSegments;
        count++;
        //REPEAT NX
    }


    for (float y = 0.5; y >= 0.25; y -= 0.25) {
        f -= y * 0.55;

        //the branch is getting smaller
        LeafBranchAB LBA(randomSeedValue, branchItterations, leafBranchPoints,
                         lineHeight, jagednessFactor_Leaf, leafVertices, nullptr, leafUVs);
        LBA.buildLeafBranch(trunkDiameter*y, count, textureLeafStart);

        //still making leaves
        LeafA LA(leafPoints, randomSeedValue, count, lineHeight, leafItterations, jagednessFactor_Leaf,
                 leafVertices, nullptr, leafUVs);
        std::cout << leafVertices->size() << " ";
        LA.buildLeaf(r1, r2, r1, count, textureLeafStart); //build two double sided leaves
        std::cout << leafVertices->size() << "------- \n ";
        lineHeight += lineSegments * y * 0.8;
        count++;
        //REPEAT 2X
    }
    LeafA LA(leafPoints, randomSeedValue, count, lineHeight, leafItterations, jagednessFactor_Leaf,
             leafVertices, leafIndices, leafUVs);
    //final top two leaves
    LA.buildLeaf(r1, r2, r1, count, textureLeafStart); //build two double sided leaves
    //Cap with a leaf
    LA.buildLeafSingle(r1, r2, r1, count, textureLeafStart);
    std::cout << leafVertices->size() << "\n";
}


void LeafContainerAB::buildLeafContainerElements(const int& start, const int& end,
                                                 std::vector<GLuint>* leafIndices, std::vector<glm::vec3>* leafVert,
                                                 std::vector<glm::vec2>* leafUVs, std::vector<glm::vec3>* leafNorms){

    const int endVert = (end - start - leafPoints*2)/ (leafBranchPoints + leafPoints*4);
    const int vertToNext = ((1) *(leafBranchPoints + leafPoints*4));
    const  int factor = (leafBranchPoints + leafPoints*4);

    GLuint i = 0;
    for(int k = 0 ; k < endVert; k++){
        std::vector<glm::vec3> surfaceNormals;
        int j = i;
        for(; i < leafBranchPoints + j ; i++) {
            const int currentVertSet = ((k) *factor);
            GLuint i1 = (i + 1) % (factor) % leafBranchPoints +  currentVertSet;

            leafIndices->push_back(i + start);
            leafIndices->push_back(i1+ start);
            leafIndices->push_back(i1 + vertToNext + start);
            leafIndices->push_back(i1  +  vertToNext + start);
            leafIndices->push_back(i  +  vertToNext + start);
            leafIndices->push_back(i+ start);
            // calculate normals
            //------------------

            std::cout <<i + start << " ";
            //step 1: find the surface normal
            glm::vec3 AB = leafVert->at(i1+ start)    -  leafVert->at(i+ start);
            glm::vec3 AC = leafVert->at(i + vertToNext +  start)    -   leafVert->at(i+ start);

            glm::vec3 surfaceNormal = -glm::cross(AB, AC);
            surfaceNormals.push_back(surfaceNormal);
        }
        //step 2: find the average of the surface normals of the surfaces this vertex is part of
        std::cout <<"-" << i <<"- ";
        int len =  surfaceNormals.size();
        for(GLuint j = 0; j < len ; j++){
            int jPos = i + start - leafBranchPoints + j + 1;
            if(jPos % leafPoints == 0) jPos =  i + start - leafBranchPoints + 0;
            std::cout <<jPos << " ";
            leafNorms->at(jPos  ) = -glm::normalize(
                    surfaceNormals.at(j) + surfaceNormals.at((j+1) % len)
            );
        }   std::cout <<"\n";


        i = LeafA::buildElements(i, start, leafPoints ,leafIndices, leafVert,
                                 leafUVs, leafNorms);
        i = LeafA::buildElements(i, start, leafPoints , leafIndices, leafVert,
                                 leafUVs, leafNorms);
    }

    //incides and normal of top leaf
    i = LeafA::buildElements(i, start,  leafPoints,  leafIndices, leafVert,
                             leafUVs, leafNorms);
    i = LeafA::buildElements(i, start,  leafPoints,  leafIndices, leafVert,
                             leafUVs, leafNorms);

    i = LeafA::buildElements(i, start,  leafPoints,  leafIndices, leafVert,
                             leafUVs, leafNorms);
}


#include "LeafAB.hpp"

//builds 2
void LeafA::buildLeaf(const float& r1, const float& r2, const float& leafDiameter, const int& count, float  leafTextureStart){
    //place leaves parralel following angle
    //place approproate leaves parralel following angle
    for (int n = 0; n < leafPoints; n++) {
        int sign = -1;
        int jagednessRandom = this->randomSeedValue * (((int)(n * 13.4) % 17) + 1);
        if (jagednessRandom % 2 == 0) sign *= -1;
        float templeafDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / leafDiameter;
        //if even create a leaf with constant Z
        if (count % 2 == 0)
            leafVertices->push_back(
                    glm::vec3(
                            r2 * cos(glm::radians(itterations  * n)) + templeafDiameter / 2.0 + r2 ,
                            lineHeight + r1 *  sin(glm::radians(itterations  * n)) ,
                            0.0f));
        //else constant X
        else
            leafVertices->push_back(
                    glm::vec3(
                    0.0f,
                    lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0,
                    r2 * cos(glm::radians(itterations  * n)) + templeafDiameter / 2.0 + r2));
    }
    unsigned  long s = leafVertices->size();
    leafUVs->resize(s);
    leafUVs->at(s - 1 - 3) = {1.0, 0 + (1 - leafTextureStart) * 0.5f};
    leafUVs->at(s - 1 - 2) = {0.5, 1 - leafTextureStart};
    leafUVs->at(s - 1 - 1) = {0,  (1 - leafTextureStart) * 0.5f};
    leafUVs->at(s - 1 - 0) = {0.5, 0};
    for (int n = 0; n < leafPoints; n++) {
        int sign = -1;
        int jagednessRandom = this->randomSeedValue * (((int)(n * 13.4) % 17) + 1);
        if (jagednessRandom % 2 == 0) sign *= -1;
        float templeafDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / leafDiameter;
        if (count % 2 == 0)
            leafVertices->push_back(
                    glm::vec3(
                            -r2 * cos(glm::radians(itterations  * n)) - templeafDiameter / 2.0 - r2 ,
                            lineHeight + r1 *  sin(glm::radians(itterations  * n)) ,
                            0.0f));
        else
            leafVertices->push_back(
                    glm::vec3(
                            0.0f,
                              lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0,
                              -r2 * cos(glm::radians(itterations  * n)) - templeafDiameter / 2.0 - r2));

    }
    s = leafVertices->size();
    leafUVs->resize(s);
    leafUVs->at(s - 1 - 3) = {1.0, 0 + (1 - leafTextureStart) * 0.5f};
    leafUVs->at(s - 1 - 2) = {0.5, 1 - leafTextureStart};
    leafUVs->at(s - 1 - 1) = {0,  (1 - leafTextureStart) * 0.5f};
    leafUVs->at(s - 1 - 0) = {0.5, 0};
}

void LeafA::buildLeafSingle(const float&  r1, const float&  r2, const float&  trunkDiameter, int y, float leafTextureStart) {
    for (int n = 0; n < leafPoints ; n++) {
        int sign = -1;
        int jagednessRandom = this->randomSeedValue * (((int) (n * 13.4) % 17) + 1);
        if (jagednessRandom % 2 == 0) sign *= -1;
        float tempTrunkDiameter = trunkDiameter +
                                  sign * (jagednessRandom) % ((int) (ceil(trunkDiameter))) * jagednessFactor_Leaf /
                                  (trunkDiameter);
        leafVertices->push_back(glm::vec3(r1 * sin(glm::radians(itterations * n)),
                                          r2 * cos(glm::radians(itterations * n)) + lineHeight + r2,
                                          0.0f));
    }
    unsigned  long s = leafVertices->size();
    leafUVs->resize(s);
    leafUVs->at(s - 1 - 3) = {1.0, 0 + (1 - leafTextureStart) * 0.5f};
    leafUVs->at(s - 1 - 2) = {0.5, 1 - leafTextureStart};
    leafUVs->at(s - 1 - 1) = {0,  (1 - leafTextureStart) * 0.5f};
    leafUVs->at(s - 1 - 0) = {0.5, 0};
}

GLuint LeafA::buildElements(int i, const int& start, const int& leafPoints,  glm::vec3 leafNormal,
                         std::vector<GLuint>* leafIndices, std::vector<glm::vec3>* leafVert,
                         std::vector<glm::vec2>* leafUVs, std::vector<glm::vec3>* leafNorms){
    if(leafVert->size() != leafNorms->size()){
        leafUVs->resize(leafVert->size());
        leafNorms->resize(leafVert->size());
    }
    //NORTH
    leafIndices->push_back(i + 0 + start);
    leafIndices->push_back(i + 1+ start);
    leafIndices->push_back(i + 2+ start);
    leafIndices->push_back(i + 2+ start);
    leafIndices->push_back(i + 3+ start);
    leafIndices->push_back(i +  0 + start);
    //NORTH
    leafIndices->push_back(i + 0 + start);
    leafIndices->push_back(i + 3 + start);
    leafIndices->push_back(i + 2 + start);
    leafIndices->push_back(i + 2 + start);
    leafIndices->push_back(i + 1 + start);
    leafIndices->push_back(i + 0 + start);

    leafNorms->at(i + 0 + start) = (leafNormal);
    leafNorms->at(i + 1 + start) = (leafNormal);
    leafNorms->at(i + 2 + start) = (leafNormal);
    leafNorms->at(i + 3 + start) = (leafNormal);

    return i = i + leafPoints;
}



LeafA::LeafA(const int& leafPoints, const int&  randomSeedValue, const int&  count, const float& lineHeight,
                      const float& itterations, const float& jagednessFactor_Leaf, std::vector<glm::vec3>* leafVertices,
                      std::vector<GLuint>* leafIndices, std::vector<glm::vec2>* leafUVs) {
    this->leafPoints = leafPoints;
    this->randomSeedValue = randomSeedValue;
    this->count = count;
    this->lineHeight = lineHeight;
    this->itterations = itterations;
    this->jagednessFactor_Leaf = jagednessFactor_Leaf;
    this->leafIndices = leafIndices;
    this->leafUVs = leafUVs;
    this->leafVertices = leafVertices;
};
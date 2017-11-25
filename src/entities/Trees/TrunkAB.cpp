#include <cmath>
#include "TrunkAB.hpp"
#include "Trunk.hpp"

TrunkAB::TrunkAB(std::vector<glm::vec3>* trunkVertices, const int& seed){
    this->trunkVertices = trunkVertices;
    baseVerticesSize = trunkVertices->size();
    this->seed = seed;
}

bool TrunkAB::buildTrunk(const float& trunkDiameter, const float& lineSegments){
    const int randomSeedValue = buildVertices(trunkDiameter, lineSegments);
    heightCount += 2;
    //make branch check
    if ((randomSeedValue * (heightCount + 1)) % branchMod == 0) {
        return false;
    }
    return true;
}

int TrunkAB::buildVertices(const float& trunkDiameter, const float& lineSegments){
    //build points
    const int randomSeedValue = TreeRandom::treeRandom(trunkDiameter, seed, lineHeight);
    for (int y = 0; y < 2; y++) {
        //disregard the UV point
        for (int n = 0; n < trunkPoints; n++) {
            int sign = -1;
            const int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1 + y);
            if (jagednessRandom % 2 == 0) sign *= -1;
            float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter)))
                                                      * jagednessFactor / (trunkDiameter);
            glm::vec3 circleEdge(
                    tempTrunkDiameter * sin(glm::radians(itterations  * n)),
                    lineHeight + 0, tempTrunkDiameter *  cos(glm::radians(itterations  * n)) );
            trunkVertices->push_back(circleEdge);
        }
        lineHeight += lineSegments;
    }
    return randomSeedValue;
}
//give both start point and end as the connection end point
void TrunkAB::buildTrunkElements(const int& start, const int& end,
                                std::vector<GLuint>* trunkIndices, std::vector<glm::vec3>* trunkVert,
                                std::vector<glm::vec2>* trunkUVs, std::vector<glm::vec3>* trunkNorms){
    if(trunkVert->size() != trunkUVs->size()){
        trunkUVs->resize(trunkVert->size());
        trunkNorms->resize(trunkVert->size());
    }
    GLuint i1 = 0;
    for (GLuint i = start; i < end -  trunkPoints + 2; i++) {
        //disregard UV vertex
        if((i+1) % trunkPoints != 0){
            //over1
            i1 = (i + 1) % (trunkPoints) + (i / trunkPoints * trunkPoints);
            trunkIndices->push_back(i);
            trunkIndices->push_back(i1);
            trunkIndices->push_back(i1 + (trunkPoints));
            trunkIndices->push_back(i1 + (trunkPoints));
            trunkIndices->push_back(i + trunkPoints);
            trunkIndices->push_back(i);
        }
        trunkNorms->push_back(glm::cross(
                trunkVert->at(i1) - trunkVert->at(i), trunkVert->at(i + trunkPoints) - trunkVert->at(i)
        ));
    }
 Trunk::computeUVs(end , start, trunkPoints, textureTrunkHeight, trunkUVs);
   
}



float TrunkAB::getLineHeight(){return lineHeight;}

void TrunkAB::buildConnectorElements(const int& segmentConnectStart, const int& start, const int& set, const char& lr,
                                   std::vector<GLuint>* trunkIndices, std::vector<glm::vec3>* trunkVert,
                                   std::vector<glm::vec2>* trunkUVs, std::vector<glm::vec3>* trunkNorms){
    if(trunkVert->size() != trunkUVs->size()){
        trunkUVs->resize(trunkVert->size());
        trunkNorms->resize(trunkVert->size());
    }

    //Norms to top segment starting from lower circle
    for(GLuint i = 0 ; i < (trunkPoints-1); i++){
        GLuint i1 = (i + 1) % (trunkPoints-1);

        trunkIndices->push_back(segmentConnectStart + (i % (trunkPoints-1)));
        trunkIndices->push_back(segmentConnectStart + (i+1) % (trunkPoints-1));
        trunkIndices->push_back(start + (i + 1 + set) % (trunkPoints-1));
        trunkIndices->push_back(start + (i + 1 + set) % (trunkPoints-1));
        trunkIndices->push_back(start + (i + set) % (trunkPoints-1));
        trunkIndices->push_back(segmentConnectStart + i % (trunkPoints-1));
        if(i % (trunkPoints-1) < (trunkPoints-1) / 2 && lr == 'R'){
            trunkNorms->at(i) = (glm::cross(
                    trunkVert->at(i1 + segmentConnectStart) - trunkVert->at(i + segmentConnectStart),
                    trunkVert->at(i + start) - trunkVert->at(i + segmentConnectStart)
            ));
        }
        else if (i % (trunkPoints-1) >= (trunkPoints-1) / 2 && lr == 'L'){
            trunkNorms->at(i) = (glm::cross(
                    trunkVert->at(i1 + segmentConnectStart) - trunkVert->at(i + segmentConnectStart),
                    trunkVert->at(i + start) - trunkVert->at(i + segmentConnectStart)
            ));
        }
    }
    computeUVsConnectors(start, (trunkPoints), textureTrunkHeight, trunkUVs);
}

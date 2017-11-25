#include <cmath>

#include "TreeB.hpp"
#include "Tree.hpp"
#include "../../loadTexture.hpp"
TreeB::TreeB(const GLuint& shader_program, Entity* entity, double trunkDiameter, int seed):
        Tree(heightChunking, boostFactor, shader_program, entity, 'B'){
    std::clock_t startTime;
    double duration;
    startTime = std::clock();

    treeLoaded = treeSetup(shader_program, trunkDiameter, seed);

    float globalRotation = TreeRandom::treeRandom(trunkDiameter,seed,widthCut*10);
    rotate(globalRotation, glm::vec3(0.0f,1.0f,0.0f));

    duration = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
    //printf("Duration of B %f Units: %f ms\n", trunkDiameter, duration*1000);
};

bool TreeB::treeSetup(const GLuint& shader_program, float trunkDiameter, const int& seed){
    draw_mode = GL_TRIANGLES;
glPointSize(5.0f);
    if (trunkDiameter <= 0.0) trunkDiameter = 1.0;
    widthCut = 0.5;
    finalCut = widthCut;

    combinedStartIndices->push_back({ -1, 0, 0, 0 });
    generateTreeB(0, trunkDiameter, seed, 0, 0, 0, 'C', nullptr, 0);
    bufferObject(shader_program);
    return true;
}

void TreeB::generateTreeB(const int& _case, float trunkDiameter, const float& seed,
                          float angleX, float angleY, float angleZ, char tag,
                          AttatchmentGroupings* ag, float lineHeight) {
    int currentLineLength = lineHeight;
    float ShootDiameterBranch = 0;
    float ShootDiameterTrunk = 0;
    glm::mat4 rotation;
    glm::vec3 translation;
    AttatchmentGroupings* agNew;
    switch (_case)
    {
        case START_TRUNK:
            //1A. Standard trunk with two branches
            //1A1. Draw circle with seed based ragedness and store in trunkV and trunkC
            //1A2. Draw circle up one with seed based ragedness and store in trunkV and trunkC
            //1A3. Draw circle up one with seed based ragedness and store in trunkV and trunkC and create trunkI
            //1A4. Make branch check
            currentLineLength = trunk(trunkDiameter, seed, currentLineLength);

            combinedStartIndices->push_back({ (int)combinedVertices->size() - 1, (int)angleX, (int)angleY, (int)angleZ});

            agNew = new AttatchmentGroupings(combinedStartIndices->at((int)combinedStartIndices->size() - 2).at(0),
                                             (int)combinedVertices->size() - 1,		//TYPE //SIDE
                                             (int)angleX, (int)angleY, (int)angleZ, 'B', 'C', lineHeight);

            //1A5. Start N new recursive functions from seed based angle at a certain base position
            ShootDiameterBranch = shootCalculation(trunkDiameter, branchRatio, branches);
            ShootDiameterTrunk = shootCalculation(trunkDiameter, trunkRatio, branches);

            //1A6. On new branch create junction and new shoot
            angleY = TreeRandom::treeOddEvenRandom(trunkDiameter, seed* ( 1) * 3, (lineHeight+1)* ( 1));
            for (int n = 0; n < branches; n++) {
                angleZ = TreeRandom::branchAngleFromRandom(trunkDiameter, seed * (n+1), currentLineLength* (n + 1), maxYBranchAngle, minYBranchAngle);
                angleX = TreeRandom::branchAngleFromRandom(trunkDiameter, seed* (n + 1) * 7, currentLineLength* (n + 1), maxYBranchAngle, minYBranchAngle); //* (((int)seed) % 2 == 0 ? -1 : 1);
                angleY = angleY;
                //1A7. On new trunk join to junction and continue
                generateTreeB(TRUNK, ShootDiameterBranch / (branches), seed, -std::abs(angleX), angleY, std::abs(angleZ), 'R', agNew, 0);
            }
            angleZ = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed* 71, currentLineLength, maxYTrunkAngle, minYTrunkAngle) ;
            angleX = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed * 9, currentLineLength, maxYTrunkAngle, minYTrunkAngle); //* (((int)seed) % 2 == 0 ? 1 : -1);
            angleY = angleY;

            generateTreeB(TRUNK, ShootDiameterTrunk, seed, std::abs(angleX), angleY, -std::abs(angleZ), 'L', agNew, currentLineLength);

            initiateMove(agNew);
            agNew->selfErase();
            delete agNew;
            break;
        case TRUNK:
            if (trunkDiameter < widthCut) {
                generateTreeB(LEAF, trunkDiameter, seed, angleX, angleY, angleZ, tag, ag, 0);
                if (trunkDiameter < finalCut) {
                    return;
                }
            }
            else{
                currentLineLength = trunk(trunkDiameter, seed, lineHeight);
            }
            //rotate based on the previous and current angles
            angleX += ag->angleX;
            angleY = angleY;
            angleZ += ag->angleZ;

            //add the sum of angles onto the current branch
            combinedStartIndices->push_back({ (int)combinedVertices->size() - 1, (int)angleX, (int)angleY, (int)angleZ });

            //store current branch poosition and rotation sum at depth
            agNew = new AttatchmentGroupings(combinedStartIndices->at((int)combinedStartIndices->size() - 2).at(0),
                                             (int)combinedVertices->size() - 1, (int)angleX, (int)angleY, (int)angleZ, 'B', tag, lineHeight);
            if (tag == 'R') ag->ag[1] = agNew;
            else			ag->ag[0] = agNew;

            //1A5. Start N new recursive functions from seed based angle at a certain base position
            angleY = TreeRandom::treeOddEvenRandom(trunkDiameter, seed * 5, lineHeight);
            if (lineHeight == -1 || currentLineLength == -1) {
                ShootDiameterTrunk = shootCalculation(trunkDiameter, trunkRatio, branches);
                currentLineLength = 0;
            }
            else{
                ShootDiameterBranch = shootCalculation(trunkDiameter, branchRatio, branches);;
                ShootDiameterTrunk = shootCalculation(trunkDiameter, trunkRatio, branches);
                for (int n = 0; n < branches; n++) {
                    //1A6. On new branch create circle then indices flowing back once to center cirlce.
                    angleZ = TreeRandom::branchAngleFromRandom(trunkDiameter, seed, currentLineLength, maxYBranchAngle, minYBranchAngle);
                    angleX = TreeRandom::branchAngleFromRandom(trunkDiameter, seed * 7, currentLineLength, maxYBranchAngle, minYBranchAngle) * (((int)seed) % 2 == 0 ? -1 : 1);;
                    angleY = angleY;
                    generateTreeB(TRUNK, ShootDiameterBranch, seed, -std::abs(angleX), angleY, std::abs(angleZ), 'R', agNew, 0);
                }
            }

            //1A7. On new trunk create circle then indices flowing back once to top circle.
            angleZ = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed, currentLineLength, minYTrunkAngle, maxYTrunkAngle);
            angleX = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed * 7, currentLineLength, minYTrunkAngle, maxYTrunkAngle) * (((int)seed) % 2 == 0 ? -1 : 1);;
            angleY = angleY;
            generateTreeB(TRUNK, ShootDiameterTrunk, seed, std::abs(angleX), angleY, -std::abs(angleZ), 'L', agNew, currentLineLength);
            break;
        case LEAF:
            //1B. If trunk width is past a threshold then create a leaf line
            //1B1.On circle put 4 oval leaves with ragedness at each 90 degrees   and store in trunkV and trunkC  and store in leafV and leafC and create leafI
            //1B2.On circle up one put 4 oval leaves with ragedness at each 90 degrees   and store in trunkV and trunkC and create trunkI  and store in leafV and leafC and create leafI
            //1B3. Repeat until max length
            leafBranch(trunkDiameter, seed, lineHeight);

            //rotate my combined of predesecors
            //rotate based on the previous and current angles
            angleX += ag->angleX;
            angleY = angleY;
            angleZ += ag->angleZ;

            //and add to leaf index with combined
            combinedStartIndices->push_back({ (int)combinedVertices->size() - 1,(int)angleX,(int)angleY,(int)angleZ });

            //add to grouping at depth
            agNew = new AttatchmentGroupings(combinedStartIndices->at((int)combinedStartIndices->size() - 2).at(0),
                                             (int)combinedVertices->size() - 1,
                                             (int)angleX, (int)angleY, (int)angleZ, 'L', tag, lineHeight);
            if (tag == 'R') ag->ag[1] = agNew;
            else			ag->ag[0] = agNew;
            //2. Translate and rotate into given location
            //get branch end, get leaf segment start get leaf segment end

            generateTreeB(END_TRUNK, trunkDiameter, seed, angleX, angleY, angleZ, tag, ag, lineHeight);
            break;
        default:
            return; break;
    }
    //AFTER FUNCTION INSIDE CONSTRUCTOR
    //3. Merge Trunk V with Leaf V into combined V
    //4. Generate rendering properties.
}

//build trunks
float TreeB::trunk(float trunkDiameter, const float& seed, float lineHeight) {
    int lineMax = lineMAX(trunkDiameter, k);
    bool loopInitialTrunk;
    //height chunking decides the size of the blocks
    float lineSegments = ((float)lineMax) / heightChunking;
    //Create a segment of a tree
    TrunkAB trunk(combinedVertices,  seed);
    int count = 0;
    do {
        loopInitialTrunk = trunk.buildTrunk(trunkDiameter, lineSegments);
    } while (loopInitialTrunk && trunk.getLineHeight() < lineMax);
    if (lineHeight >= lineMax)
        return -1;
    else if(!loopInitialTrunk)
        return lineHeight;
    else return -1;
}

void TreeB::leafBranch(float trunkDiameter, const float& seed, float lineHeight) {
    int lineMax = lineMAX(trunkDiameter, k);
    LeafContainerAB lc(combinedVertices,
                      combinedIndices,
                      combinedUV,
                      seed);
    lc.buildContainer(trunkDiameter, seed, lineHeight, lineMax);
}

//PUT TEXTURE LOADING IN SEPERATE CLASS. MAKE IT ONLY CALLED ONCE FOR THE FIRST TREE LOADED.
void TreeB::bufferObject(const GLuint& shader_program) {
    this->vao = initVertexArray( *combinedVertices, *combinedIndices, *combinedNormals, *combinedUV,
                                 &vbo, &ebo, &nbo, &UVbo);
}

int limiter = 1;
void TreeB::initiateMove(AttatchmentGroupings* ag){
    glm::mat4 rotation;
    int circularPoints = TrunkAB::trunkPoints -1;
    int rotationPoint = std::abs((ag->angleY) % (int)((circularPoints) / limiter ));

    rotationPoint = rotationPoint == 0 ? 1 : 0;

    const float r = 360.0/circularPoints  * (rotationPoint);
    int start = ag->start + 1;
    int max = ag->end + 1;
    for (int k = start; k < max; k++) {
        combinedVertices->at(k)  = makeRotations(glm::radians((float)ag->angleX), glm::radians(r),glm::radians((float)ag->angleZ),
                                                 combinedVertices->at(k));
    }
    int previousRotation = rotationPoint;
    Tree::computeElements(ag);
    moveSegments(previousRotation, ag);
    return;
}

void TreeB::moveSegments(const int& previousRotation, AttatchmentGroupings* ag) {
    for (int m = 0; m < 2; m++) {
        if (ag->ag[m] == nullptr) continue;
        int moveTo = 0;
        int moveFrom = 0;

        if(previousRotation < previousRotationCap) limiter = 1;
        else if(previousRotation >= previousRotationCap) limiter = 0.01;

        int circularPoints = ag->ag[m]->type == 'L' ? LeafContainerAB::leafBranchPoints - 1: TrunkAB::trunkPoints - 1;
        int rotationPoint = std::abs((ag->ag[m]->angleY) % (int)(circularPoints / limiter ));

        //""mess up the structure a bit for varition""
        rotationPoint = rotationPoint < 1 ? 1 : 0;
        const int toPnt = (rotationPoint + (previousRotation));
        const int fromPnt = (previousRotation);

        if (ag->ag[m]->side == 'L') {
            ///Because UV vertex
            moveTo = (ag->end - (circularPoints + 1) + 1) + ((2 + toPnt) % circularPoints);
            moveFrom = (ag->ag[m]->start + 1)  + ((0 + fromPnt) % circularPoints);
        }
        else {
            ///Because UV vertex
            moveTo = (ag->end - (circularPoints + 1) + 1) + (int)(circularPoints/2.0 + toPnt ) % circularPoints;
            moveFrom = (ag->ag[m]->start + 1) + (int)(circularPoints/2.0  + fromPnt) % circularPoints;
        }

        const float r = 360.0f/circularPoints  * (toPnt);

        const  int start = ag->ag[m]->start + 1;
        const int max = ag->ag[m]->end + 1;

        for (int k = start; k < max; k++) {
            combinedVertices->at(k) = makeRotations( glm::radians((float)ag->ag[m]->angleX), glm::radians(r),
                                                     glm::radians((float)ag->ag[m]->angleZ), combinedVertices->at(k));
        }

        //translate components onto branch(destination - position)
        const glm::vec3 translation = combinedVertices->at(moveTo) - combinedVertices->at(moveFrom);
        //elevate from point
        const glm::vec3 boost = boostSegment(ag, ag->ag[m], combinedVertices) *  (float)(heightChunking * boostFactor);
        for (int k = start; k < max; k++) {
            combinedVertices->at(k) += translation + boost;
        }
        computeElements(ag->ag[m]);
        connectSegments(ag, m,toPnt, fromPnt, circularPoints, k,  combinedIndices);
        moveSegments(toPnt, ag->ag[m]);
    }
    return;
}

const std::vector<glm::vec3>& TreeB::getVertices()
{
    return  *combinedVertices;
}

GLuint TreeB::getVAO()
{
    return this->vao;
}

GLuint TreeB::getTextureId()
{
    static GLuint tA_texture = loadTexture(
            "../textures/TreeALeafTest.jpg",//1000Y break // 925X break
            GL_NEAREST,
            GL_NEAREST
    );
    return tA_texture;
}

const int TreeB::getColorType() {
    return COLOR_TEXTURE;
}


//treeA
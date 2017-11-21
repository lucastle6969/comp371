#include "TreeA.hpp"

void TreeA::generateTreeA(int _case, float trunkDiameter, float seed, float angleX, float angleY, float angleZ, char tag, AttatchmentGroupings* ag, float lineHeight) {
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
                                             (int)angleX, (int)angleY, (int)angleZ, 'B', 'C');

            //1A5. Start N new recursive functions from seed based angle at a certain base position
            ShootDiameterBranch = ShootCalculation(trunkDiameter, branchRatio);
            ShootDiameterTrunk = ShootCalculation(trunkDiameter, trunkRatio);

            //1A6. On new branch create junction and new shoot
            angleY = TreeRandom::treeOddEvenRandom(trunkDiameter, seed* ( 1) * 3, (lineHeight+1)* ( 1));
            for (int n = 0; n < branches; n++) {
                angleZ = TreeRandom::branchAngleFromRandom(trunkDiameter, seed * (n+1), currentLineLength* (n + 1), maxYBranchAngle, minYBranchAngle);
                angleX = TreeRandom::branchAngleFromRandom(trunkDiameter, seed* (n + 1) * 7, currentLineLength* (n + 1), maxYBranchAngle, minYBranchAngle); //* (((int)seed) % 2 == 0 ? -1 : 1);
                angleY = angleY;
                //CONSIDERATION MULTITHREAD THE BRANCH AND MOVEMENT
                generateTreeA(TRUNK, ShootDiameterBranch / (branches), seed, -abs(angleX), angleY, abs(angleZ), 'R', agNew, 0);

                //////////// REQUIREMENT: ALTER COMBINE FUNCTION
            }
            //1A7. On new trunk join to junction and continue
            angleZ = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed* 71, currentLineLength, maxYTrunkAngle, minYTrunkAngle) ;
            angleX = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed * 9, currentLineLength, maxYTrunkAngle, minYTrunkAngle); //* (((int)seed) % 2 == 0 ? 1 : -1);
            angleY = angleY;

            //CONSIDERATION: MULTITHREAD THE TRUNK AND MOVEMENT
            generateTreeA(TRUNK, ShootDiameterTrunk, seed, abs(angleX), angleY, -abs(angleZ), 'L', agNew, currentLineLength);

            initiateMove(0, agNew);
            agNew->selfErase();
            delete agNew;
            //////////// REQUIREMENT: ALTER COMBINE FUNCTION
            break;
        case TRUNK:
            if (trunkDiameter < widthCut) {
                generateTreeA(LEAF, trunkDiameter, seed, angleX, angleY, angleZ, tag, ag, 0);
                if (trunkDiameter < finalCut) {
                    return;
                }
            }
            else{
                currentLineLength = trunk(trunkDiameter, seed, lineHeight);
            }
            //TODAY     ------------------------------------------------

            //rotate based on the previous and current angles
            angleX += ag->angleX;
            angleY = angleY;
            angleZ += ag->angleZ;

            //add the sum of angles onto the current branch
            combinedStartIndices->push_back({ (int)combinedVertices->size() - 1, (int)angleX, (int)angleY, (int)angleZ });

            //store current branch poosition and rotation sum at depth
            agNew = new AttatchmentGroupings(combinedStartIndices->at((int)combinedStartIndices->size() - 2).at(0),
                                             (int)combinedVertices->size() - 1, (int)angleX, (int)angleY, (int)angleZ, 'B', tag);
            if (tag == 'R') ag->ag[1] = agNew;
            else			ag->ag[0] = agNew;

            //1A5. Start N new recursive functions from seed based angle at a certain base position
            angleY = TreeRandom::treeOddEvenRandom(trunkDiameter, seed * 5, lineHeight);
            if (lineHeight == -1 || currentLineLength == -1) {
                //if(//Test)//("====SPLIT====\n");
                ShootDiameterTrunk = ShootCalculation(trunkDiameter, trunkRatio);
                currentLineLength = 0;
            }
            else{
                ShootDiameterBranch = ShootCalculation(trunkDiameter, branchRatio);
                ShootDiameterTrunk = ShootCalculation(trunkDiameter, trunkRatio);
                for (int n = 0; n < branches; n++) {
                    //1A6. On new branch create circle then indices flowing back once to center cirlce.
                    angleZ = TreeRandom::branchAngleFromRandom(trunkDiameter, seed, currentLineLength, maxYBranchAngle, minYBranchAngle);
                    angleX = TreeRandom::branchAngleFromRandom(trunkDiameter, seed * 7, currentLineLength, maxYBranchAngle, minYBranchAngle) * (((int)seed) % 2 == 0 ? -1 : 1);;
                    angleY = angleY;
                    generateTreeA(TRUNK, ShootDiameterBranch, seed, -abs(angleX), angleY, abs(angleZ), 'R', agNew, 0);
                }
            }

            //1A7. On new trunk create circle then indices flowing back once to top circle.
            angleZ = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed, currentLineLength, minYTrunkAngle, maxYTrunkAngle);
            angleX = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed * 7, currentLineLength, minYTrunkAngle, maxYTrunkAngle) * (((int)seed) % 2 == 0 ? -1 : 1);;
            angleY = angleY;
            generateTreeA(TRUNK, ShootDiameterTrunk, seed, abs(angleX), angleY, -abs(angleZ), 'L', agNew, currentLineLength);
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
                                             (int)angleX, (int)angleY, (int)angleZ, 'L', tag);
            if (tag == 'R') ag->ag[1] = agNew;
            else			ag->ag[0] = agNew;
            //2. Translate and rotate into given location
            //get branch end, get leaf segment start get leaf segment end

            generateTreeA(END_TRUNK, trunkDiameter, seed, angleX, angleY, angleZ, tag, ag, lineHeight);
            break;
        default:
            return; break;
    }
    //AFTER FUNCTION INSIDE CONSTRUCTOR
    //3. Merge Trunk V with Leaf V into combined V
    //4. Generate rendering properties.
}

float TreeA::trunk(float trunkDiameter, float seed, float lineHeight) {
    int lineMax = lineMAX(trunkDiameter);
    bool loopInitialTrunk;
    float lineSegments = ((float)lineMax) / heightChunking;
    TrunkA trunk(combinedVertices,
                 combinedIndices,
                 combinedColor,
                 combinedStartIndices,
                 combinedUV,
                 seed
    );
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

int countL = 0;
void TreeA::leafBranch(float trunkDiameter, float seed, float lineHeight) {
    int lineMax = lineMAX(trunkDiameter);
    LeafContainerA lc(combinedVertices,
                      combinedIndices,
                      combinedColor,
                      combinedStartIndices,
                      combinedUV,
                      seed);
    lc.buildContainer(trunkDiameter, seed, lineHeight, lineMax);
}

//TODO: ADD SHADER: DARKER ON MORE Z USING TEXTURES IN SHADER
//PUT TEXTURE LOADING IN SEPERATE CLASS. MAKE IT ONLY CALLED ONCE FOR THE FIRST TREE LOADED.
void TreeA::bufferObject(const GLuint& shader_program) {
    //this->vao = Entity::initVertexArray(shader_program, this->combinedNormals, 0);
    //int map_width, map_height, channels;
    //unsigned char * image_data = stbi_load("../wall.jpg", &map_width, &map_height, &channels, STBI_rgb);
    this->vao = initVertexArray( *combinedVertices, *combinedIndices, &vbo, &ebo);
    //stbi_image_free(image_data);
}

void TreeA::initiateMove(int pastRot, AttatchmentGroupings* ag){
    glm::mat4 rotation;
    int circularPoints = TrunkA::trunkPoints;
    int rotationPoint = abs((ag->angleY) % (int)(circularPoints / limiter ));

    rotationPoint = rotationPoint == 0 ? 0 : 1;

    float r = 360.0/circularPoints  * (rotationPoint);
    int start = ag->start + 1;
    int max = ag->end + 1;
    for (int k = start; k < max; k++) {
        combinedVertices->at(k)  = makeRotations(glm::radians((float)ag->angleX), glm::radians(r),glm::radians((float)ag->angleZ),
                                                 combinedVertices->at(k));
    }
    int previousRotation = rotationPoint;
    computeElementsInitial(ag);
    moveSegments(previousRotation, ag);
    return;
}
void TreeA::moveSegments(int previousRotation, AttatchmentGroupings* ag) {
    for (int m = 0; m < 2; m++) {
        if (ag->ag[m] == nullptr) continue;
        int moveTo = 0;
        int moveFrom = 0;

        if(previousRotation < previousRotationCap) limiter = 1;
        else if(previousRotation >= previousRotationCap) limiter = 0.01;

        int circularPoints = ag->ag[m]->type == 'L' ? LeafContainerA::leafBranchPoints : TrunkA::trunkPoints;
        int rotationPoint = abs((ag->ag[m]->angleY) % (int)(circularPoints / limiter ));

        //shrub like
        //if(previousRotation == 0) rotationPoint = 1;

        rotationPoint = rotationPoint < 1 ? 1 : 0;

        //printf("RP%d PR%d %d %% %d\n", rotationPoint, previousRotation,ag->ag[m]->angleY, circularPoints / limiter);

        int toPnt = (circularPoints-rotationPoint + (previousRotation));
        int fromPnt = (previousRotation);

//            std::cout <<  ag->ag[m]->angleZ << "Z ''' ";
//            std::cout <<  fromPnt % 4 << ", "<< ag->ag[m]->side <<" "<< (toPnt + 2) % 4 <<" ''' ";
//            std::cout <<  ag->ag[m]->angleX << "X ====";

        if (ag->ag[m]->side == 'L') {
            moveTo = (ag->end - circularPoints + 1) + (( 0 + toPnt) % circularPoints);
            moveFrom = (ag->ag[m]->start + 1)  + ((0 + fromPnt) % circularPoints);

        }
        else {
            moveTo = (ag->end - circularPoints + 1) + (int)(circularPoints/2.0 + toPnt ) % circularPoints;
            moveFrom = (ag->ag[m]->start + 1) + (int)(circularPoints/2.0  + fromPnt) % circularPoints;
        }

        float r = 360.0/circularPoints  * (toPnt);

//            ag->ag[m]->angleX = (rotationPoint + previousRotation) % circularPoints == 4 ?
//                    (ag->ag[m]->angleX < 0 ? ag->ag[m]->angleX += 90:
//                     (ag->ag[m]->angleX > 0? ag->ag[m]->angleX -= 90:ag->ag[m]->angleX)):
//                        ag->ag[m]->angleX;
//            ag->ag[m]->angleZ = (rotationPoint + previousRotation) % circularPoints == 2 ?
//                                (ag->ag[m]->angleZ < 0 ? ag->ag[m]->angleZ += 90:
//                                 (ag->ag[m]->angleZ > 0? ag->ag[m]->angleZ -= 90:ag->ag[m]->angleZ += 90)):
//                                    ag->ag[m]->angleZ;

//            std::cout <<  ag->ag[m]->angleX << "\n";


        int start = ag->ag[m]->start + 1;
        int max = ag->ag[m]->end + 1;
        ////("%d, %d\n",start, max);

        for (int k = start; k < max; k++) {
            combinedVertices->at(k) = makeRotations( glm::radians((float)ag->ag[m]->angleX), glm::radians(r),
                                                     glm::radians((float)ag->ag[m]->angleZ), combinedVertices->at(k));
        }

        //translate components onto branch(destination - position)
        glm::vec3 translation = combinedVertices->at(moveTo) - combinedVertices->at(moveFrom);
        //elevate from point
        glm::vec3 boost = boostSegment(ag, ag->ag[m], combinedVertices) *  (float)(heightChunking * boostFactor);
        start = ag->ag[m]->start + 1;
        max = ag->ag[m]->end + 1;
        for (int k = start; k < max; k++) {
            combinedVertices->at(k) += translation + boost;
        }
        computeElementsInitial(ag->ag[m]);
        connectSegments(ag, m,toPnt, fromPnt, circularPoints, combinedIndices);
        moveSegments(toPnt, ag->ag[m]);
    }
    return;

}

bool TreeA::treeSetup(const GLuint& shader_program, float trunkDiameter, float seed){
    draw_mode = GL_TRIANGLES;
    if (trunkDiameter <= 0.0) trunkDiameter = 1.0;
    widthCut = 0.5;//trunkDiameter / 32;
    finalCut = widthCut;

    combinedStartIndices->push_back({ -1, 0, 0, 0 });
    generateTreeA(0, trunkDiameter, seed, 0, 0, 0, 'C', nullptr, 0);
    bufferObject(shader_program);
    return true;
}


const std::vector<glm::vec3>& TreeA::getVertices()
{
    return  *combinedVertices;
}


GLuint TreeA::getVAO()
{
    return this->vao;
}

const int TreeA::getColorType(){return 0;};

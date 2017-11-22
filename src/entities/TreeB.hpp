/*
____DOCUMENTATION_OF_TREE_GENERATION_____

ONE CENTRAL .CPP FILE CONTROLS GENERATION OF TREE STRUCTURE. IS BROKEN DOWN INTO A LOG COMPONENT AND A LEAF BRANCH COMPONENT THAT
ARE MESHED TOGETHER USING A RECURISIVE ALGORITHM STORE COMBINED VERTICES(THROUGH POINTERS)
	TreeA(const GLuint& shader_program, float trunkDiameter, float seed)
CALL TO GENERATE OBJECT WILL RESULT IN AN OBJECT WITH A STORED VAO IN THESE PARAMETERS.
CREATES TRUNK AND LEAF VECTORS+INDICES WHICH ARE MERGED TOGETHER ARE SENT TO THE GPU.

CURRENT LOADING SPEED SUGESTS THE 4-8 DIAMETER RANGE WILL GENERATE QUICKLY. SCALING TO HEIGHTS MAY BE NESCICARY.

PROGRAM ENTRY POINT IN IN THE generateTreeA FUNCTION WHEREIN IT STARTS TO BUILD THE STARTER TRUNK,
THEN ENTERS BINARY RECURSIONS OF ITSELF TO GENERATE THE TRUNKS AND LEAVES WHEREUPON REACHING THE BASE CASE IN MOVES SEGMENTS OF
ONE RECURSION ONTO THE STARTER TRUNK AND PROCEEDS TO DO THE NEXT BRANCH. RECURSIONS SET PARAMETERS OF ANGLES AND SEGMENT WIDTHS
DECREASING AT A PYTHAGOREAN RATE.

TRUNKS HAVE FOUR PROPERTIES.
  1) THE WIDTH OF THE SEGMENT
  2) THE JAGEDNESS(BUMPYNESS) OF THE SEGMENT
  3) THE MAX HEIGHT IT CAN REACH
  4) THE SEED RELATED TO IT
THEY ARE STRAIGHT LINE PIECES OF ROUGHLY CIRCULAR SIZE

LEAF BRANCHES HAVE FOUR PROPERTIES
  1) THE WIDTH OF THE BRANCH SEGMENT
  2) THE JAGEDNESS(BUMPYNESS) OF THE SEGMENT'S LEAVES
  3) THE MAX HEIGHT IT CAN REACH
  4) THE SEED RELATED TO IT
AT EVERY PART OF THE BRANCH IT SPLITS  INTO TWO LEAVES ALTERNATING IN PERPENDICULARS.
AT THE LAST 2 PARTS THE LEAVES GET SMALLER UNTIL THE LAST WHERE IT ENDS WITH A LEAF.
LEAFS ARE DOUBLE SIDED FOR BACKFACE CULLING.

SEGMENTS ARE STORED INTO ATTATCHMENT GROUPINGS AS A LINKED LIST AND ARE STICHED TOGETHER IN THE MOVE PROCESS USING THE
ANGLES COMPUTED DURING RECURSIONS
*/

#ifndef COMP371_TREE_B_HPP
#define COMP371_TREE_B_HPP

#define START_TRUNK 0
#define TRUNK 2
#define LEAF 3
#define END_TRUNK -1


#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <iostream>
#include <cstdio>
#include <ctime>

#include "../TreeRandom.hpp"
#include "TrunkA.hpp"
#include "LeafContainerA.hpp"

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Tree.hpp"

class TreeB : public Tree {
private:

    //CONSIDERATION FOR MULTITHREADED LOADING
    bool treeLoaded = false;

    const int branches = 1;
    const int k = 1;

    static constexpr float boostFactor = 0.5;
    static constexpr int heightChunking = 12;//INVERSE

    const int minYBranchAngle = 0;
    const int maxYBranchAngle = 70;
    const int minYTrunkAngle = 0;
    const int maxYTrunkAngle = 70;

    double trunkRatio = 1.0;
    double branchRatio = 0.850;

    float ShootCalculation(float trunkDiameter, double ratio){	return pow(pow(trunkDiameter, 2) / (branches + 1), 1.0 / 2.0) * ratio;	}
    int lineMAX(float trunkDiameter) { return ceil(pow(pow(trunkDiameter, 2) * k, 1.0 / 2.0)); }

    void generateTreeB(int _case, float trunkDiameter, float seed, float angleX, float angleY, float angleZ, char tag, AttatchmentGroupings* ag, float lineHeight) {
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
                    generateTreeB(TRUNK, ShootDiameterBranch / (branches), seed, -abs(angleX), angleY, abs(angleZ), 'R', agNew, 0);

                    //////////// REQUIREMENT: ALTER COMBINE FUNCTION
                }
                //1A7. On new trunk join to junction and continue
                angleZ = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed* 71, currentLineLength, maxYTrunkAngle, minYTrunkAngle) ;
                angleX = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed * 9, currentLineLength, maxYTrunkAngle, minYTrunkAngle); //* (((int)seed) % 2 == 0 ? 1 : -1);
                angleY = angleY;

                //CONSIDERATION: MULTITHREAD THE TRUNK AND MOVEMENT
                generateTreeB(TRUNK, ShootDiameterTrunk, seed, abs(angleX), angleY, -abs(angleZ), 'L', agNew, currentLineLength);

                initiateMove(0, agNew);
                agNew->selfErase();
                delete agNew;
                //////////// REQUIREMENT: ALTER COMBINE FUNCTION
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
                        generateTreeB(TRUNK, ShootDiameterBranch, seed, -abs(angleX), angleY, abs(angleZ), 'R', agNew, 0);
                    }
                }

                //1A7. On new trunk create circle then indices flowing back once to top circle.
                angleZ = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed, currentLineLength, minYTrunkAngle, maxYTrunkAngle);
                angleX = TreeRandom::trunkAngleFromRandom(trunkDiameter, seed * 7, currentLineLength, minYTrunkAngle, maxYTrunkAngle) * (((int)seed) % 2 == 0 ? -1 : 1);;
                angleY = angleY;
                generateTreeB(TRUNK, ShootDiameterTrunk, seed, abs(angleX), angleY, -abs(angleZ), 'L', agNew, currentLineLength);
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

                generateTreeB(END_TRUNK, trunkDiameter, seed, angleX, angleY, angleZ, tag, ag, lineHeight);
                break;
            default:
                return; break;
        }
        //AFTER FUNCTION INSIDE CONSTRUCTOR
        //3. Merge Trunk V with Leaf V into combined V
        //4. Generate rendering properties.
    }

    int bCount = 0;
    float trunk(float trunkDiameter, float seed, float lineHeight) {
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
    void leafBranch(float trunkDiameter, float seed, float lineHeight) {
        int lineMax = lineMAX(trunkDiameter);
        LeafContainerA lc(combinedVertices,
                          combinedIndices,
                          combinedColor,
                          combinedStartIndices,
                          combinedUV,
                          seed);
        lc.buildContainer(trunkDiameter, seed, lineHeight, lineMax);
    }

    //PUT TEXTURE LOADING IN SEPERATE CLASS. MAKE IT ONLY CALLED ONCE FOR THE FIRST TREE LOADED.
    void bufferObject(const GLuint& shader_program) {
        //this->vao = Entity::initVertexArray(shader_program, this->combinedNormals, 0);
        //int map_width, map_height, channels;
        //unsigned char * image_data = stbi_load("../wall.jpg", &map_width, &map_height, &channels, STBI_rgb);
        this->vao = initVertexArray( *combinedVertices, *combinedIndices, &vbo, &ebo);
        //stbi_image_free(image_data);
    }

    int limiter = 1;
    void initiateMove(int pastRot, AttatchmentGroupings* ag){
        glm::mat4 rotation;
        int circularPoints = TrunkA::trunkPoints;
        int rotationPoint = abs((ag->angleY) % (circularPoints / limiter ));

        rotationPoint = rotationPoint == 0 ? 1 : 0;

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
    void moveSegments(int previousRotation, AttatchmentGroupings* ag) {
        for (int m = 0; m < 2; m++) {
            if (ag->ag[m] == nullptr) continue;
            int moveTo = 0;
            int moveFrom = 0;

            int circularPoints = ag->ag[m]->type == 'L' ? LeafContainerA::leafBranchPoints : TrunkA::trunkPoints;
            int rotationPoint = abs((ag->ag[m]->angleY) % (circularPoints / limiter ));

            //shrub like
            if(previousRotation == 0) rotationPoint = 1;

            rotationPoint = rotationPoint < 1 ? 0 : 1;

            //printf("RP%d PR%d %d %% %d\n", rotationPoint, previousRotation,ag->ag[m]->angleY, circularPoints / limiter);

            int fromPnt = (circularPoints-rotationPoint + (previousRotation));
            int toPnt = (previousRotation);

//            std::cout <<  ag->ag[m]->angleZ << " ''' ";
//            std::cout <<  fromPnt % 4 << ", " << (toPnt + 2) % 4 <<" ''' ";
//            std::cout <<  ag->ag[m]->angleX << "X ==== ";

            if (ag->ag[m]->side == 'L') {
                moveTo = (ag->end - circularPoints + 1) + (( 2 + toPnt) % circularPoints);
                moveFrom = (ag->ag[m]->start + 1)  + ((2 + fromPnt) % circularPoints);

            }
            else {
                moveTo = (ag->end - circularPoints + 1) + (int)(0 + toPnt ) % circularPoints;
                moveFrom = (ag->ag[m]->start + 1) + (int)(0  + fromPnt) % circularPoints;
            }

            float r = 360.0/circularPoints  * (fromPnt);

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
            glm::vec3 boost = boostSegment(ag, ag->ag[m], combinedVertices) *  (heightChunking * boostFactor);
            start = ag->ag[m]->start + 1;
            max = ag->ag[m]->end + 1;
            for (int k = start; k < max; k++) {
                combinedVertices->at(k) += translation + boost;
            }
            computeElementsInitial(ag->ag[m]);
            connectSegments(ag, m,fromPnt, toPnt, circularPoints, combinedIndices);
            moveSegments(fromPnt, ag->ag[m]);
        }
        return;

    }


    bool treeSetup(const GLuint& shader_program, float trunkDiameter, float seed){
        draw_mode = GL_TRIANGLES;
        if (trunkDiameter <= 0.0) trunkDiameter = 1.0;
        widthCut = 0.5;//trunkDiameter / 32;
        finalCut = widthCut;

        combinedStartIndices->push_back({ -1, 0, 0, 0 });
        generateTreeB(0, trunkDiameter, seed, 0, 0, 0, 'C', nullptr, 0);
        bufferObject(shader_program);
        return true;
    }



public:
    const std::vector<glm::vec3>& getVertices()
    {
        return  *combinedVertices;
    }

    GLuint getVAO()
    {
        return this->vao;
    }

    TreeB(const GLuint& shader_program, Entity* entity, double trunkDiameter, int seed):
            Tree(heightChunking, boostFactor, shader_program, entity, 'B'){
        std::clock_t startTime;
        double duration;
        startTime = std::clock();

        //CONSIDERATION: MULTITHREAD THE GENERATION TO NOT INTERFERE WITH GAME LOOP
        treeLoaded = treeSetup(shader_program, trunkDiameter, seed);

        float globalRotation = TreeRandom::treeRandom(trunkDiameter,seed,widthCut*10);
        rotate(globalRotation, glm::vec3(0.0f,1.0f,0.0f));


        duration = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
        printf("Duration of B %f Units: %f ms\n", trunkDiameter, duration*1000);
    };
};
#endif //treeA
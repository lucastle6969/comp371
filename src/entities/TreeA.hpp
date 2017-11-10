/*
NOTES FOR POTENTIAL LARGE ALTERATIONS: 
	REMOVE ALL THE RECURSION. ITTERATION IS FASTER.
	DONE BY BEFORE DUE DATE? EXPERIMENT ON MULTITHREADING
*/

/*
TODO: READ NOTATION OF TODO, CONSIDERATION AND SPLIT FOR OBJECTIVES. MASTER TODO- SPLIT ALL CLASSES FROM INLINE TO CPP HPP COMBINATION.

____DOCUMENTATION_OF_TREE_GENERATION_____

ONE MASTER .HPP FILE CONTROLS GENERATION OF TREE STRUCTURE. CAN BE SPLIT INTO INDIVIDUAL COMPONENTS IF NEED BE.
MIGHT CONSIDER MULTITHREADING THE TWO MAIN BRANCH SEGMENTS IF CONTEXT SWITCHING IS NOT TOO EXPENSIVE.

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
AT EVERY PART OF THE BRANCH IT SPLITS OFF INTO TWO LEAVES ALTERNATING IN PERPENDICULARS.
AT THE LAST 2 PARTS THE LEAVES GET SMALLER UNTIL THE LAST WHERE IT ENDS WITH A LEAF.
LEAFS ARE DOUBLE SIDED FOR BACKFACE CULLING.

SEGMENTS ARE STORED INTO ATTATCHMENT GROUPINGS AS A LINKED LIST AND ARE STICHED TOGETHER IN THE MOVE PROCESS USING THE 
ANGLES COMPUTED DURING RECURSIONS
*/

//#ifndef COMP371_TREE_A_HPP
#define COMP371_TREE_A_HPP

#define START_TRUNK 0
#define BRANCH 1
#define TRUNK 2
#define LEAF 3
#define TOP_SEGMENT 4
#define END_TRUNK -1

#define GROUP_A1 0
#define GROUP_A2 1

#ifdef __APPLE__
#include <Opengl3.h>
#include <Opengl3ext.h>
#else
#include <glew.h> // include GL Extension Wrangler
#endif

#include <gtc/matrix_transform.hpp>
#include <glm.hpp>

#include <vector>

#include "entities/entity.hpp"
#include "constants.hpp"
#include "vendor/stb_image.h"

class TreeA : public Entity {
private:

	bool printfTest = false;
	bool breakbool;

	class AttatchmentGroupings {
	public:
		//START END ANGLEX ANGLEY ANGLEZ
		int start;
		int end;
		int angleX;
		int angleY;
		int angleZ;
		char type;
		char side;

		AttatchmentGroupings* ag[2];
		
		AttatchmentGroupings(int start,	int end, int angleX, int angleY, int angleZ,char type,	char side){
			this->start = start;
			this->end = end;
			this->angleX = angleX;
			this->angleY = angleY;
			this->angleZ = angleZ;
			this->type = type;
			this->side = side;
			ag[0] = nullptr; ag[1] = nullptr;
		}
		void selfErase() {
			for (int i = 0; i < 2; i++) {
				if (this->ag[i] != nullptr) {
					this->ag[i]->selfErase();
					delete this->ag[i];
					this->ag[i] = nullptr;
				}
			}
		}
	};
		
	//CONSIDERATION FOR MULTITHREADED LOADING
	bool treeLoaded = false;
	
	std::vector<AttatchmentGroupings> branchStore;
	std::vector<glm::vec3> combinedVertices; 	std::vector<GLuint> combinedIndices;		std::vector<glm::vec3> combinedColor; std::vector<glm::vec3> combinedNormals; std::vector<glm::vec2> combinedUV;
	GLuint vao; GLuint vbo; GLuint ebo; GLuint tbo;

	const int branches = 1;

	//MOVE TO TRUNK CLASS
	std::vector<glm::vec3> trunkVertices; 	std::vector<GLuint> trunkIndices;	std::vector<glm::vec3> trunkColor; std::vector<std::vector<int>> trunkStartIndices;
	const int circlePoints = 4;
	const float jagednessFactor = 0.45;
	glm::vec3 brown = glm::vec3(0.0 / 255, 53.0 / 255, 10.0 / 255);
	
	//MOVE TO LEAF CLASS
	const int leafPoints = 5;
	const float jagednessFactor_Leaf = 0.25;
	std::vector<glm::vec3> leafVertices; 	std::vector<GLuint> leafIndices;		std::vector<glm::vec3> leafColor; std::vector<std::vector<int>> leafStartIndices;
	glm::vec3 green = glm::vec3(30.0 / 255, 147.0 / 255, 45.0 / 255);


	const int k = 250;
	float widthCutoff;
	float finalCutoff;
	bool kill;

	const int branchMod = 10;
	const int minYBranchAngle = 25;
	const int maxYBranchAngle = 45;
	const int minYTrunkAngle = 0;
	const int maxYTrunkAngle = 25;
	const int yRotMod = 360;
	int heightChunking = 10;//INVERSE

	double trunkRatio = 1.0;
	double branchRatio = 0.850;

	int currentAttatchment = 1;
	int depth = 0;
	int depthMax = 0;

	int treeRandom(float trunkDiameter, float seed, float lineHeight) {
		int randomSeedValue = ((int)(trunkDiameter * seed * (((int)lineHeight % 4 * (int)lineHeight % 10 + 1 ) * 10) * 3)) % (77 * (int)ceil(trunkDiameter));
		if (randomSeedValue % 2 == 0) randomSeedValue = -(randomSeedValue + 1);
		return randomSeedValue;
	}
	int treeOddEvenRandom(float trunkDiameter, float seed, float lineHeight) {
		int randomSeedValue = ((int)(trunkDiameter * seed * (((int)lineHeight % 4 * (int)lineHeight % 10 + 1) * 10) * 3)) % (77 * (int)ceil(trunkDiameter));
		return randomSeedValue;
	}
	int branchAngleFromRandom(float trunkDiameter, float seed, float lineHeight) {return treeRandom(trunkDiameter, seed, lineHeight) % (maxYBranchAngle - minYBranchAngle) + minYBranchAngle;}
	int trunkAngleFromRandom(float trunkDiameter, float seed, float lineHeight) { return treeRandom(trunkDiameter, seed, lineHeight) % (maxYTrunkAngle - minYTrunkAngle) + minYTrunkAngle; }
	float offShootCalculation(float trunkDiameter, double ratio){
			return pow(pow(trunkDiameter, 2) / (branches + 1), 1.0 / 2.0) * ratio;
	}
	int lineMAX(float trunkDiameter) { return ceil(pow(pow(trunkDiameter, 2) * k, 1.0 / 2.0)); }

	void generateTreeA(int _case, float trunkDiameter, float seed, float angleX, float angleY, float angleZ, char tag, AttatchmentGroupings* ag, float lineHeight) {
		int currentLineLength = lineHeight;
		float offShootDiameterBranch = 0;
		float offShootDiameterTrunk = 0;
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
			currentLineLength = starterTrunk(trunkDiameter, seed, currentLineLength);
			trunkStartIndices.push_back({ (int)trunkVertices.size() - 1, (int)angleX, (int)angleY, (int)angleZ});

			agNew = new AttatchmentGroupings(trunkStartIndices.at((int)trunkStartIndices.size() - 2).at(0), (int)trunkVertices.size() - 1, (int)angleX, (int)angleY, (int)angleZ, 'B', tag);

			//1A5. Start N new recursive functions from seed based angle at a certain base position
			offShootDiameterBranch = offShootCalculation(trunkDiameter, branchRatio);
			offShootDiameterTrunk = offShootCalculation(trunkDiameter, trunkRatio);

			//1A6. On new branch create junction and new shoot
			angleY = treeOddEvenRandom(trunkDiameter, seed* ( 1) * 3, (lineHeight+1)* ( 1));
			for (int n = 0; n < branches; n++) {
				angleZ = branchAngleFromRandom(trunkDiameter, seed * (n+1), currentLineLength* (n + 1));
				angleX = branchAngleFromRandom(trunkDiameter, seed* (n + 1) * 7, currentLineLength* (n + 1)); //* (((int)seed) % 2 == 0 ? -1 : 1);
				angleY = angleY;
				depth = 0;
				//CONSIDERATION MULTITHREAD THE BRANCH AND MOVEMENT
				generateTreeA(TRUNK, offShootDiameterBranch / (branches), seed, -abs(angleX), angleY, angleZ, 'R', agNew, 0);
				breakbool = true;
				moveSegments(-abs(angleX), angleZ, angleY, agNew);
				agNew->selfErase();
				//////////// REQUIREMENT: ALTER COMBINE FUNCTION
			}
			//1A7. On new trunk join to junction and continue
			angleZ = trunkAngleFromRandom(trunkDiameter, seed, currentLineLength) ;
			angleX = trunkAngleFromRandom(trunkDiameter, seed * 9, currentLineLength); //* (((int)seed) % 2 == 0 ? 1 : -1);
			angleY = angleY;
			depth = 0;
			globalRotations = 0;
			//CONSIDERATION: MULTITHREAD THE TRUNK AND MOVEMENT
			generateTreeA(TRUNK, offShootDiameterTrunk, seed, abs(angleX), angleY, angleZ, 'L', agNew, currentLineLength);
			moveSegments(abs(angleX), angleZ, angleY, agNew);
			agNew->selfErase();
			delete agNew;
			//////////// REQUIREMENT: ALTER COMBINE FUNCTION
			break;   
		case TRUNK:
				depth++;
				kill = false;
				if (trunkDiameter < widthCutoff) {
					generateTreeA(LEAF, trunkDiameter, seed, angleX, angleY, angleZ, tag, ag, 0);
					if (trunkDiameter < finalCutoff) {
						depthMax = depth;
						depth--;
						kill = true;
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
			trunkStartIndices.push_back({ (int)trunkVertices.size() - 1, (int)angleX, (int)angleY, (int)angleZ });

			//store current branch poosition and rotation sum at depth
			agNew = new AttatchmentGroupings(trunkStartIndices.at((int)trunkStartIndices.size() - 2).at(0), (int)trunkVertices.size() - 1, (int)angleX, (int)angleY, (int)angleZ, 'B', tag);
			if (tag == 'R') ag->ag[1] = agNew;
			else			ag->ag[0] = agNew;
					
			//1A5. Start N new recursive functions from seed based angle at a certain base position
			angleY = treeRandom(trunkDiameter, seed * 5, lineHeight);
			if (lineHeight == -1 || currentLineLength == -1) {
				if(printfTest)printf("====SPLIT====\n");
				offShootDiameterTrunk = offShootCalculation(trunkDiameter, trunkRatio);
				lineHeight = 0; currentLineLength = 0;
			}
			else{
				offShootDiameterBranch = offShootCalculation(trunkDiameter, branchRatio);
				offShootDiameterTrunk = offShootCalculation(trunkDiameter, trunkRatio);
				for (int n = 0; n < branches; n++) {
					//1A6. On new branch create circle then indices flowing back once to center cirlce.
					angleZ = -branchAngleFromRandom(trunkDiameter, seed, currentLineLength);
					angleX = branchAngleFromRandom(trunkDiameter, seed * 7, currentLineLength) * (((int)seed) % 2 == 0 ? -1 : 1);;
					angleY = angleY;
					generateTreeA(TRUNK, offShootDiameterBranch, seed, -abs(angleX), angleY, angleZ, 'R', agNew, 0);
				}
			}

			//1A7. On new trunk create circle then indices flowing back once to top circle.
			angleZ = trunkAngleFromRandom(trunkDiameter, seed, currentLineLength);
			angleX = trunkAngleFromRandom(trunkDiameter, seed * 7, currentLineLength) * (((int)seed) % 2 == 0 ? -1 : 1);;
			angleY = angleY;
			generateTreeA(TRUNK, offShootDiameterTrunk, seed, abs(angleX), angleY, angleZ, 'L', agNew, currentLineLength);
			depth--;
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
			leafStartIndices.push_back({ (int)leafVertices.size() - 1,(int)angleX,(int)angleY,(int)angleZ });

			//add to grouping at depth
			agNew = new AttatchmentGroupings(leafStartIndices.at((int)leafStartIndices.size() - 2).at(0), (int)leafVertices.size() - 1, (int)angleX, (int)angleY, (int)angleZ, 'L', tag);
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

	//Return the line length where it meets a possitive branch check
	float starterTrunk(float trunkDiameter, float seed, float lineHeight) {
		int randomSeedValue = treeRandom(trunkDiameter, seed, lineHeight);
		int lineMax = lineMAX(trunkDiameter);
		bool loopInitialTrunk = true;
		float lineSegments = ((float)lineMax) / heightChunking;
		int baseVerticesSize = trunkVertices.size();
		int count = 0;
		//int offL = 50 + countL * 40;
		//int off = 50;//+ bCount * 5;
		int offL = 0;
		int off = 0;
		
		//MOVE TO PROCEDURE TRUNK CLASS
		while(loopInitialTrunk && lineHeight < lineMax){
			//build points
			float itterations = 360.0 / circlePoints;
			for(int y = 0; y < 3; y++){
				for (int n = 0; n < circlePoints; n++) {
					int sign = -1;
					int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1 + y);
					if (jagednessRandom % 2 == 0) sign *= -1;
					float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter))) * jagednessFactor / (trunkDiameter);
					trunkVertices.push_back(glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations  * n)) , lineHeight, tempTrunkDiameter *  cos(glm::radians(itterations  * n))));
					trunkColor.push_back(brown);
				}
				lineHeight += lineSegments;
				randomSeedValue = treeRandom(trunkDiameter, seed, lineHeight);
			}
			//build indices && normals
			for (int y = 0; y < 2; y++) {
				for (int n = 0; n < circlePoints; n++) {
					int offset = (y + 1) * circlePoints;
					int base = y * circlePoints;
					if (n == circlePoints - 1) {
						trunkIndices.push_back(base + n + count * circlePoints); trunkIndices.push_back(base + count * circlePoints); trunkIndices.push_back(offset + count * circlePoints);
						trunkIndices.push_back(offset + count * circlePoints); trunkIndices.push_back(offset + n + count * circlePoints);  trunkIndices.push_back(base + n + count * circlePoints);
					}
					else {
						//WORKS
						trunkIndices.push_back(base + n + count * circlePoints); trunkIndices.push_back(base + n + 1 + count * circlePoints); trunkIndices.push_back(offset + n + 1 + count * circlePoints);
						trunkIndices.push_back(offset + n + 1 + count * circlePoints); trunkIndices.push_back(offset + n + count * circlePoints);  trunkIndices.push_back(base + n + count * circlePoints);
					}
					if (count > 0 && y == 0) {
						int countTemp = count - 1;
						if (n == circlePoints - 1) {
							trunkIndices.push_back(base + n + countTemp * circlePoints); trunkIndices.push_back(base + countTemp * circlePoints); trunkIndices.push_back(offset + countTemp * circlePoints);
							trunkIndices.push_back(offset + countTemp * circlePoints); trunkIndices.push_back(offset + n + countTemp * circlePoints);  trunkIndices.push_back(base + n + countTemp * circlePoints);
						}
						else {
							trunkIndices.push_back(base + n + countTemp * circlePoints); trunkIndices.push_back(base + n + 1 + countTemp * circlePoints); trunkIndices.push_back(offset + n + 1 + countTemp * circlePoints);
							trunkIndices.push_back(offset + n + 1 + countTemp * circlePoints); trunkIndices.push_back(offset + n + countTemp * circlePoints); trunkIndices.push_back(base + n + countTemp * circlePoints);
						}
					}
				}			
			}
			
			count += 3;

			//make branch check 
			if ((randomSeedValue * (count + 1)) % branchMod == 0) loopInitialTrunk = false;
		}
		if (printfTest)printf("%d\n", (randomSeedValue * (count + 1)) % branchMod);
		return lineHeight;
	}	
	
	int bCount = 0;
	float trunk(float trunkDiameter, float seed, float lineHeight) {
		int randomSeedValue = treeRandom(trunkDiameter, seed, lineHeight);
		int lineMax = lineMAX(trunkDiameter);
		bool loopInitialTrunk = true;
		float lineSegments = ((float)lineMax) / heightChunking;
		int baseVerticesSize = trunkVertices.size();
		int count = 0;
		//int offL = 50 + countL * 40;
		//int off = 50;//+ bCount * 5;
		int offL = 0;
		int off = 0;
		
		//MOVE PROCEEDURE TO TRUNK CLASS
		do {
			//build points
			float itterations = 360.0 / circlePoints;
			for (int y = 0; y < 3; y++) {
				for (int n = 0; n < circlePoints; n++) {
					int sign = -1;
					int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1 + y);
					if (jagednessRandom % 2 == 0) sign *= -1;
					float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter))) * jagednessFactor / (trunkDiameter);
					glm::vec3 circleEdge(tempTrunkDiameter * sin(glm::radians(itterations  * n)) + off, lineHeight + 0, tempTrunkDiameter *  cos(glm::radians(itterations  * n)) + offL);
					trunkVertices.push_back(circleEdge);
					trunkColor.push_back(brown);
				}
				lineHeight += lineSegments;
				randomSeedValue = treeRandom(trunkDiameter, seed, lineHeight);
			}
			//build indices
			for (int y = 0; y < 2; y++) {
				for (int n = 0; n < circlePoints; n++) {
					int offset = (y + 1) * circlePoints + baseVerticesSize;
					int base = y * circlePoints + baseVerticesSize;
					if (n == circlePoints - 1) {
						trunkIndices.push_back(base + n + count * circlePoints); trunkIndices.push_back(base + count * circlePoints); trunkIndices.push_back(offset + count * circlePoints);
						trunkIndices.push_back(offset + count * circlePoints); trunkIndices.push_back(offset + n + count * circlePoints);  trunkIndices.push_back(base + n + count * circlePoints);
					}
					else {
						//WORKS
						trunkIndices.push_back(base + n + count * circlePoints); trunkIndices.push_back(base + n + 1 + count * circlePoints); trunkIndices.push_back(offset + n + 1 + count * circlePoints);
						trunkIndices.push_back(offset + n + 1 + count * circlePoints); trunkIndices.push_back(offset + n + count * circlePoints);  trunkIndices.push_back(base + n + count * circlePoints);
					}
					if (count > 0 && y == 0) {
						int countTemp = count - 1;
						if (n == circlePoints - 1) {
							trunkIndices.push_back(base + n + countTemp * circlePoints); trunkIndices.push_back(base + countTemp * circlePoints); trunkIndices.push_back(offset + countTemp * circlePoints);
							trunkIndices.push_back(offset + countTemp * circlePoints); trunkIndices.push_back(offset + n + countTemp * circlePoints);  trunkIndices.push_back(base + n + countTemp * circlePoints);
						}
						else {
							trunkIndices.push_back(base + n + countTemp * circlePoints); trunkIndices.push_back(base + n + 1 + countTemp * circlePoints); trunkIndices.push_back(offset + n + 1 + countTemp * circlePoints);
							trunkIndices.push_back(offset + n + 1 + countTemp * circlePoints); trunkIndices.push_back(offset + n + countTemp * circlePoints); trunkIndices.push_back(base + n + countTemp * circlePoints);
						}
					}
				}
			}

			count += 3;

			//make branch check 
			//printf("%d\n", (randomSeedValue * (count + 1)) % branchMod);
			if ((randomSeedValue * (count + 1)) % branchMod == 0) {
				if (printfTest)printf("BRANCH COUNT: %d\n", ++bCount);
				loopInitialTrunk = false;
			}
		} while (loopInitialTrunk && lineHeight < lineMax);
		if (lineHeight >= lineMax)
			return -1;
		else if(!loopInitialTrunk)
			return lineHeight;
		else return -1;
	}

	int countL = 0;
	void leafBranch(float trunkDiameter, float seed, float lineHeight) {
		int randomSeedValue = treeRandom(trunkDiameter, seed, lineHeight);
		int lineMax = lineMAX(trunkDiameter);
		bool loopInitialTrunk = true;
		float lineSegments = trunkDiameter * 3.0;//((float)lineMax) / heightChunking;
		int baseVerticesSize = leafVertices.size();
		int count = 0;
		float itterations = 360.0 / leafPoints;
		int offL = 50 + countL * 40;
		int off = 50 + bCount * 5 + offL;

		//MOVE PROCEEDURE TO LEAF CLASS
		//CONDENSE ALGORITHM SIZE
		float f = 2;

		f = 1;

		while (lineMax % 3 != 0) lineMax--;

		float r2 = 0.000176135 * sqrt(abs(-88063572 + 50843527 * trunkDiameter * lineSegments)) * f;
		float r1 = sqrt(3) * sqrt(abs(2 - r2 * r2)) * f;

		while (lineHeight < lineMax - 2){
			for (int n = 0; n < leafPoints ; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter))) * jagednessFactor / (trunkDiameter);
				leafVertices.push_back(glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations  * n)) , lineHeight + 0, tempTrunkDiameter *  cos(glm::radians(itterations  * n)) + off));
			}
		//place leaves parralel following angle
			//place approproate leaves parralel following angle
			for (int n = 0; n < leafPoints; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / trunkDiameter;
				if (count % 2 == 0)
					leafVertices.push_back(glm::vec3(r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , off));
				else
					leafVertices.push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 + off));

			}
			for (int n = 0; n < leafPoints; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / trunkDiameter;
				if (count % 2 == 0)
					leafVertices.push_back(glm::vec3(-r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , off));
				else
					leafVertices.push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, -r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 + off));

			}
			lineHeight += lineSegments;
			count++;
		//REPEAT NX	
		}

		for (float y = 0.5; y >= 0.25; y -= 0.25) {
			f -= y * 0.55;
			//Generate 1U circle rotated downwards
			for (int n = 0; n < leafPoints; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = trunkDiameter * y + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter* y))) * jagednessFactor / (trunkDiameter* y);
				leafVertices.push_back(glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations  * n)) , lineHeight + 0, tempTrunkDiameter *  cos(glm::radians(itterations  * n)) + off));

			}
			//place leaves parralel following angle
			//float r2 = 0.000176135 * sqrt(abs(-88063572 + 50843527 * trunkDiameter * lineSegments))* f;
			//float r1 = sqrt(3) * sqrt(abs(2 - r2 * r2))* f;
			//place approproate leaves parralel following angle
			for (int n = 0; n < leafPoints; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / (trunkDiameter* y);
				if (count % 2 == 0)
					leafVertices.push_back(glm::vec3(r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , off));
				else
					leafVertices.push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 + off));

			}
			for (int n = 0; n < leafPoints; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / (trunkDiameter* y);
				if (count % 2 == 0)
					leafVertices.push_back(glm::vec3(-r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , off));
				else
					leafVertices.push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, -r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 + off));

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
						leafIndices.push_back(base + n); leafIndices.push_back(base + 0); leafIndices.push_back(offset + 0);
						leafIndices.push_back(offset + 0);  leafIndices.push_back(offset + n); leafIndices.push_back(base + n);
					}
					else {
						leafIndices.push_back(base + n); leafIndices.push_back(base + n + 1); leafIndices.push_back(offset + n + 1);
						leafIndices.push_back(offset + n + 1);  leafIndices.push_back(offset + n); leafIndices.push_back(base + n);
					}
				}
			}
			else {
				for (int n = 0; n < leafPoints - 2; n++) {
					int base = i * leafPoints * 3 + baseVerticesSize;
					if (n == leafPoints - 2 - 1) {
						leafIndices.push_back(base); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2);  leafIndices.push_back(base - n); leafIndices.push_back(base);
					}
					else {
						leafIndices.push_back(base); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2);  leafIndices.push_back(base + 3); leafIndices.push_back(base);
					}
					if (n == leafPoints - 2 - 1) {
						leafIndices.push_back(base); leafIndices.push_back(base - n); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base);
					}
					//topsegment cover
					else {
						leafIndices.push_back(base); leafIndices.push_back(base + 3); leafIndices.push_back(base + 2);
					}
				}
			}
			//leaf1&2 N and S side
			for (int j = 0; j < 2; j++) {
				for (int n = 0; n < leafPoints - 2; n++) {
					int base = leafPoints * (j + 1) + i * leafPoints * 3 + baseVerticesSize + n;
					//NORTH
					if (n == leafPoints - 2 - 1) {
						leafIndices.push_back(base); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2);  leafIndices.push_back(base - n); leafIndices.push_back(base);
					}
					else {
						leafIndices.push_back(base); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2);  leafIndices.push_back(base + 3); leafIndices.push_back(base);
					}
					//SOUTH
					if (n == leafPoints - 2 - 1) {
						leafIndices.push_back(base); leafIndices.push_back(base - n); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base);
					}
					else {
						leafIndices.push_back(base); leafIndices.push_back(base + 3); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base);
					}
				}
			}
		}

		//Cap with a leaf
		//r2 = 0.000176135 * sqrt(abs(-88063572 + 50843527 * trunkDiameter * lineSegments))* f;
		//r1 = sqrt(3) * sqrt(abs(2 - r2 * r2)) * f;
		for (int n = 0; n < leafPoints ; n++) {
			int sign = -1;
			int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
			if (jagednessRandom % 2 == 0) sign *= -1;
			float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter))) * jagednessFactor_Leaf / (trunkDiameter);
			leafVertices.push_back(glm::vec3(r1 * sin(glm::radians(itterations  * n)) , r2 *  cos(glm::radians(itterations  * n)) + lineHeight + r2, 0 + off));
			leafColor.push_back(brown);
		}
		for (int n = 0; n < leafPoints - 2; n++) {
			int base = leafVertices.size() - leafPoints + n;
			if (n == leafPoints - 2 - 1) {
				leafIndices.push_back(base); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
				leafIndices.push_back(base + 2);  leafIndices.push_back(base - n); leafIndices.push_back(base);
			}
			else {
				leafIndices.push_back(base); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
				leafIndices.push_back(base + 2);  leafIndices.push_back(base + 3); leafIndices.push_back(base);
			}
			if (n == leafPoints - 2 - 1) {
				leafIndices.push_back(base); leafIndices.push_back(base - n); leafIndices.push_back(base + 2);
				leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base);
			}
			else {
				leafIndices.push_back(base); leafIndices.push_back(base + 3); leafIndices.push_back(base + 2);
				leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base);
			}
		}
		//conect branches
		//conect leaves

		//NOTE: LEAVE BRANCHES COME IN PAIRS
		countL++;
		offL = 50 + countL * 40;
		off = 50 + bCount * 5 + offL;
		if (printfTest)printf("LEAF COUNT: %d\n", countL);
	}

	int globalRotations = 0;
	void moveSegments(float aX, float aZ, int rotationPoint, AttatchmentGroupings* ag) {
		glm::vec3 translation;
		glm::mat4 rotation;
		for (int m = 0; m < 2; m++) {
			if (ag->ag[m] == nullptr) continue;

			int moveTo = 0;
			int moveFrom = 0;

			int circularPoints = ag->ag[m]->type == 'L' ? leafPoints : circlePoints;
			rotationPoint = abs(ag->ag[m]->angleY % (circularPoints));

			if (ag->ag[m]->side == 'L') {
				globalRotations += rotationPoint;
				moveTo = (ag->end - circularPoints + 1) + 2;//((2 + globalRotations - rotationPoint) % circularPoints);
				moveFrom = (ag->ag[m]->start + 1) + 2;//((2 + rotationPoint) % circularPoints);

			}
			else {
				moveTo = (ag->end - circularPoints + 1) + 0;//(0 + globalRotations) % 4;
				moveFrom = (ag->ag[m]->start + 1) + 0;// (0 + globalRotations - rotationPoint) % 4;
			}
			//TODO: create rotations of branches
			float r = 360.0  * (0*4);
			rotation = glm::rotate(rotation, glm::radians((float)r), glm::vec3(0.0, -1.0, 0.0));
			rotation = glm::rotate(rotation, glm::radians((float)ag->ag[m]->angleX), glm::vec3(1.0, 0.0, 0.0));
			rotation = glm::rotate(rotation, glm::radians((float)ag->ag[m]->angleZ), glm::vec3(0.0, 0.0, -1.0));

			if (ag->ag[m]->type == 'B') {
				int start = ag->ag[m]->start + 1;
				int max = ag->ag[m]->end + 1;
				for (int k = start; k < max; k++) {
					trunkVertices.at(k) = glm::vec4(trunkVertices.at(k), 1.0f) * rotation;
				}
			}
			else {
				int start = ag->ag[m]->start + 1;
				int max = ag->ag[m]->end + 1;
				for (int k = start; k < max; k++) {
					leafVertices.at(k) = glm::vec4(leafVertices.at(k), 1.0f) * rotation;
				}
			}

			//translate components onto branch(destination - position)
			std::vector<glm::vec3>* vContainer2 = nullptr;
			if (ag->ag[m]->type == 'B') vContainer2 = &trunkVertices;
			else vContainer2 = &leafVertices;
			std::vector<glm::vec3>* vContainer1 = nullptr;
			if (ag->type == 'B') vContainer1 = &trunkVertices;
			else vContainer1 = &leafVertices;
			translation = vContainer1->at(moveTo) - vContainer2->at(moveFrom);

			if (ag->ag[m]->type == 'B') {
				int start = ag->ag[m]->start + 1;
				int max = ag->ag[m]->end + 1;
				for (int k = start; k < max; k++) {
					trunkVertices.at(k) += translation;
				}
			}
			else {
				int start = ag->ag[m]->start + 1;
				int max = ag->ag[m]->end + 1;
				for (int k = start; k < max; k++) {
					leafVertices.at(k) += translation;
				}
			}

		connectSegments(ag->ag[m]);
		moveSegments(ag->ag[m]->angleX, ag->ag[m]->angleZ, ag->ag[m]->angleY, ag->ag[m]);
		globalRotations -= rotationPoint;
	}
		return;
		
	}

	//TODO: COMPLETE CONECTORS
	void connectSegments(AttatchmentGroupings* ag){
		depth--;
		//if(ag[1].type == 'B'){
		//	//go through each point on upper part of at depth circle
		//	for (int i = 1; i < 4; i += 2) {
		//		//connect to upper circle 1
		//		if(ag[1].ag[0]->type == 'B'){
		//			for (int j = 0; j < circlePoints; j++) {
		//				if (j == circlePoints - 1) {
		//					int A = ag[1].groupings.at(0).at(0) - circlePoints + 1 + j;
		//					int B = ag[1].groupings.at(0).at(0) - circlePoints + 1 + (0);
		//					int C = ag[1].ag[0].groupings.at(0).at(0) + 1 + (0);
		//					trunkIndices.push_back(A);
		//					trunkIndices.push_back(B);
		//					trunkIndices.push_back(C);
		//					int D = C;
		//					int E = ag[1].ag[0].groupings.at(0).at(0) + 1 + (j);
		//					int F = A;
		//					trunkIndices.push_back(D);
		//					trunkIndices.push_back(E);
		//					trunkIndices.push_back(F);
		//				}
		//				else {
		//					int A = ag[1].groupings.at(0).at(0) - circlePoints + 1 + j;
		//					int B = ag[1].groupings.at(0).at(0) - circlePoints + 1 + (j + 1);
		//					int C = ag[1].ag[0].groupings.at(0).at(0) + 1 + (j + 1);
		//					trunkIndices.push_back(A);
		//					trunkIndices.push_back(B);
		//					trunkIndices.push_back(C);
		//					int D = C;
		//					int E = ag[1].ag[0].groupings.at(0).at(0) + 1 + (j);
		//					int F = A;
		//					trunkIndices.push_back(D);
		//					trunkIndices.push_back(E);
		//					trunkIndices.push_back(F);
		//				}
		//			}
		//		}
		//	}
		//}
	}

	//TODO
	void computeUV() {

	}
	//TODO
	void computeNormal() {

	}

	void combineVectors() {
				//TEST: IS IT FASTER WITH WHILE LOOP OR SOMETHING ELSE?
		combinedVertices.reserve(trunkVertices.size() + leafVertices.size());
		combinedVertices.insert(combinedVertices.end(), trunkVertices.begin(), trunkVertices.end());
		combinedVertices.insert(combinedVertices.end(), leafVertices.begin(), leafVertices.end());

		//CONSIDER MERGING WITH FUNCTION BELOW?
		std::vector<GLuint> ind;
		int  size = trunkVertices.size();
		int len = leafIndices.size();
		int i = 0;
		while (i < len) {
			ind.push_back(leafIndices.at(i) + size);
			i++;
		}
		leafIndices = ind;

				//TEST: IS IT FASTER WITH WHILE LOOP OR SOMETHING ELSE?
		combinedIndices.reserve(trunkIndices.size() + leafIndices.size());
		combinedIndices.insert(combinedIndices.end(), trunkIndices.begin(), trunkIndices.end());
		combinedIndices.insert(combinedIndices.end(), leafIndices.begin(), leafIndices.end());

				//TEST: IS IT FASTER WITH WHILE LOOP OR SOMETHING ELSE?
		combinedColor.reserve(trunkColor.size() + leafColor.size());
		combinedColor.insert(combinedColor.end(), trunkColor.begin(), trunkColor.end());
		combinedColor.insert(combinedColor.end(), leafColor.begin(), leafColor.end());
	}

		//TODO: ADD SHADER: DARKER ON MORE Z USING TEXTURES IN SHADER
		//PUT TEXTURE LOADING IN SEPERATE CLASS. MAKE IT ONLY CALLED ONCE FOR THE FIRST TREE LOADED.
	void bufferObject(const GLuint& shader_program) {
		//this->vao = Entity::initVertexArray(shader_program, this->combinedNormals, 0);
		//int map_width, map_height, channels;
		//unsigned char * image_data = stbi_load("../wall.jpg", &map_width, &map_height, &channels, STBI_rgb);
		this->vao = Entity::initVertexArray(shader_program, this->combinedVertices, this->combinedIndices, &vbo, &ebo);
		//stbi_image_free(image_data);
	}	
	
	bool treeSetup(const GLuint& shader_program, float trunkDiameter, float seed){
		generateTreeA(0, trunkDiameter, seed, 0, 0, 0, 'C', nullptr, 0);
		computeNormal();
		computeUV();
		combineVectors();
		bufferObject(shader_program);
	}

public:
	const std::vector<glm::vec3>& getVertices()
	{
		return this->combinedVertices;
	}

	const int getColorType()
	{
		return COLOR_WHITE;
	}

	GLuint getVAO()
	{
		return this->vao;
	}

#include <iostream>
#include <cstdio>
#include <ctime>

	TreeA(const GLuint& shader_program, float trunkDiameter, float seed){
		std::clock_t startTime;
		double duration;

		startTime = std::clock();

		this->draw_mode = GL_TRIANGLES;
		
		if (trunkDiameter <= 0.0) trunkDiameter = 1.0;
		widthCutoff = 0.5;//trunkDiameter / 16;
		finalCutoff = widthCutoff;

		leafStartIndices.push_back({ -1, 0, 0, 0 });
		trunkStartIndices.push_back({ -1, 0, 0, 0 });
		
		//CONSIDERATION: MULTITHREAD THE GENERATION TO NOT INTERFERE WITH GAME LOOP
		treeLoaded = treeSetup(const GLuint& shader_program, float trunkDiameter, float seed);


		duration = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
		printf("Duration %f: %f\n", trunkDiameter, duration);
	};

	TreeA() {};
};
//
//glm::vec3 U = leafVertices.at(base + n+1) - leafVertices.at(base + n);
//glm::vec3 V = leafVertices.at(offset + n) - leafVertices.at(base + n);
//glm::vec3 normal = glm::normalize(glm::cross(U, V));
//combinedNormals.push_back(normal);

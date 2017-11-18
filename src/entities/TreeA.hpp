/*
 * TODO:   Move code into files   Create Normals lined with verts     UV placement to line with Vert       Branching angle Bug     branch <-> leaves connect		||  merge  || 			Textures        Tree Variations(no rotation double, regular, shrub patch[0 alpha lines])        Texture Variations(Oak, Birch, pine)
 *																														 Requires regularly combining indices
 *

NOTES FOR POTENTIAL LARGE ALTERATIONS: 
	REMOVE ALL THE RECURSION. ITTERATION IS FASTER.
	DONE BY BEFORE DUE DATE? EXPERIMENT ON MULTITHREADING?
*/

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

//#ifndef COMP371_TREE_A_HPP
#define COMP371_TREE_A_HPP

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

#include "Entity.hpp"
#include "DrawableEntity.hpp"

class TreeA : public DrawableEntity {
private:

	bool printTest = false;
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
			for (AttatchmentGroupings* agI: this->ag){
				if (agI != nullptr) {
					agI->selfErase();
					delete agI;
					agI = nullptr;
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
	std::vector<glm::vec2> trunkUVs;
	const int circlePoints = 4;
	const float jagednessFactor = 0.35;
	const float textureTrunkHeight = 0.9;
	glm::vec3 brown = glm::vec3(0.0 / 255, 53.0 / 255, 10.0 / 255);

	//MOVE TO LEAF CLASS
	const int leafPoints = 5;
	const float jagednessFactor_Leaf = 0.35;
	const float textureLeafStart = textureTrunkHeight;
	const float textureLeafEnd = 1.0f;
	std::vector<glm::vec3> leafVertices; 	std::vector<GLuint> leafIndices;		std::vector<glm::vec3> leafColor; std::vector<std::vector<int>> leafStartIndices;
	std::vector<glm::vec2> leafUVs;
	glm::vec3 green = glm::vec3(30.0 / 255, 147.0 / 255, 45.0 / 255);


	const int k = 250;
	float widthCut;
	float finalCut;

	const int branchMod = 5;
	const int minYBranchAngle = 25;
	const int maxYBranchAngle = 45;
	const int minYTrunkAngle = 0;
	const int maxYTrunkAngle = 25;
	int heightChunking = 10;//INVERSE

	double trunkRatio = 1.0;
	double branchRatio = 0.850;

	int depth = 0;

	int treeRandom(float trunkDiameter, float seed, float lineHeight) {
		int randomSeedValue = ((int)(trunkDiameter * seed * (((int)lineHeight % 10 * (int)(lineHeight*7) % 100 + 1 ) * 10) * 3)) % (k*10 * (int)ceil(trunkDiameter));
		if (randomSeedValue % 2 == 0) randomSeedValue = -(randomSeedValue + 1);
		return randomSeedValue;
	}
	int treeOddEvenRandom(float trunkDiameter, float seed, float lineHeight) {
		int randomSeedValue = ((int)(trunkDiameter * seed * (((int)lineHeight % 10 * (int)(lineHeight*7) % 100 + 1 ) * 10) * 3)) % (k*10 * (int)ceil(trunkDiameter));
		return randomSeedValue;
	}
	int branchAngleFromRandom(float trunkDiameter, float seed, float lineHeight) {return treeRandom(trunkDiameter, seed, lineHeight) % (maxYBranchAngle - minYBranchAngle) + minYBranchAngle;}
	int trunkAngleFromRandom(float trunkDiameter, float seed, float lineHeight) { return treeRandom(trunkDiameter, seed, lineHeight) % (maxYTrunkAngle - minYTrunkAngle) + minYTrunkAngle; }
	float ShootCalculation(float trunkDiameter, double ratio){
			return pow(pow(trunkDiameter, 2) / (branches + 1), 1.0 / 2.0) * ratio;
	}
	int lineMAX(float trunkDiameter) { return ceil(pow(pow(trunkDiameter, 2) * k, 1.0 / 2.0)); }

	void generateTreeA(int _case, float trunkDiameter, float seed, float angleX, float angleY, float angleZ, char tag, AttatchmentGroupings* ag, float lineHeight) {
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
			trunkStartIndices.push_back({ (int)trunkVertices.size() - 1, (int)angleX, (int)angleY, (int)angleZ});

			agNew = new AttatchmentGroupings(trunkStartIndices.at((int)trunkStartIndices.size() - 2).at(0), (int)trunkVertices.size() - 1, (int)angleX, (int)angleY, (int)angleZ, 'B', tag);

			//1A5. Start N new recursive functions from seed based angle at a certain base position
			ShootDiameterBranch = ShootCalculation(trunkDiameter, branchRatio);
			ShootDiameterTrunk = ShootCalculation(trunkDiameter, trunkRatio);

			//1A6. On new branch create junction and new shoot
			angleY = treeOddEvenRandom(trunkDiameter, seed* ( 1) * 3, (lineHeight+1)* ( 1));
			for (int n = 0; n < branches; n++) {
				angleZ = branchAngleFromRandom(trunkDiameter, seed * (n+1), currentLineLength* (n + 1));
				angleX = branchAngleFromRandom(trunkDiameter, seed* (n + 1) * 7, currentLineLength* (n + 1)); //* (((int)seed) % 2 == 0 ? -1 : 1);
				angleY = angleY;
				depth = 0;
				//CONSIDERATION MULTITHREAD THE BRANCH AND MOVEMENT
				generateTreeA(TRUNK, ShootDiameterBranch / (branches), seed, -abs(angleX), angleY, abs(angleZ), 'R', agNew, 0);
				breakbool = true;

				//////////// REQUIREMENT: ALTER COMBINE FUNCTION
			}
			//1A7. On new trunk join to junction and continue
			angleZ = trunkAngleFromRandom(trunkDiameter, seed* 71, currentLineLength) ;
			angleX = trunkAngleFromRandom(trunkDiameter, seed * 9, currentLineLength); //* (((int)seed) % 2 == 0 ? 1 : -1);
			angleY = angleY;
			depth = 0;
			//CONSIDERATION: MULTITHREAD THE TRUNK AND MOVEMENT
			generateTreeA(TRUNK, ShootDiameterTrunk, seed, abs(angleX), angleY, -abs(angleZ), 'L', agNew, currentLineLength);

            initiateMove(0, agNew);
			agNew->selfErase();
			delete agNew;
			//////////// REQUIREMENT: ALTER COMBINE FUNCTION
			break;
		case TRUNK:
				depth++;
				if (trunkDiameter < widthCut) {
					generateTreeA(LEAF, trunkDiameter, seed, angleX, angleY, angleZ, tag, ag, 0);
					if (trunkDiameter < finalCut) {
						depth--;
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
			angleY = treeOddEvenRandom(trunkDiameter, seed * 5, lineHeight);
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
					angleZ = branchAngleFromRandom(trunkDiameter, seed, currentLineLength);
					angleX = branchAngleFromRandom(trunkDiameter, seed * 7, currentLineLength) * (((int)seed) % 2 == 0 ? -1 : 1);;
					angleY = angleY;
					generateTreeA(TRUNK, ShootDiameterBranch, seed, -abs(angleX), angleY, abs(angleZ), 'R', agNew, 0);
				}
			}

			//1A7. On new trunk create circle then indices flowing back once to top circle.
			angleZ = trunkAngleFromRandom(trunkDiameter, seed, currentLineLength);
			angleX = trunkAngleFromRandom(trunkDiameter, seed * 7, currentLineLength) * (((int)seed) % 2 == 0 ? -1 : 1);;
			angleY = angleY;
			generateTreeA(TRUNK, ShootDiameterTrunk, seed, abs(angleX), angleY, -abs(angleZ), 'L', agNew, currentLineLength);
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

	int bCount = 0;
	float trunk(float trunkDiameter, float seed, float lineHeight) {
		int randomSeedValue = treeRandom(trunkDiameter, seed, lineHeight);
		int lineMax = lineMAX(trunkDiameter);
		bool loopInitialTrunk = true;
		float lineSegments = ((float)lineMax) / heightChunking;
		int baseVerticesSize = trunkVertices.size();
		int count = 0;

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
					glm::vec3 circleEdge(tempTrunkDiameter * sin(glm::radians(itterations  * n)), lineHeight + 0, tempTrunkDiameter *  cos(glm::radians(itterations  * n)) );
					trunkVertices.push_back(circleEdge);
					trunkColor.push_back(brown);
				}
				lineHeight += lineSegments;
				randomSeedValue = treeRandom(trunkDiameter, seed, lineHeight);
			}
			//build indices && UVs
			int yMax = 2;
			int ySoluton = 0;
			for (int y = 0; y < yMax; y++) {
				int uvYMax = yMax;
				int nSolution  = 0;
				for (int n = 0; n < circlePoints; n++) {
					int set = (ySoluton + y + 1) * circlePoints + baseVerticesSize;
					int base = (ySoluton+ y) * circlePoints+ baseVerticesSize;
					if (count > 0 && y == 0) {
						int countTemp = count - 1;
						uvYMax = yMax + 1;
						if (n == circlePoints - 1) {
							trunkIndices.push_back(base + n + countTemp * circlePoints); trunkIndices.push_back(base + countTemp * circlePoints); trunkIndices.push_back(set + countTemp * circlePoints);
							trunkIndices.push_back(set + countTemp * circlePoints); trunkIndices.push_back(set + n + countTemp * circlePoints);  trunkIndices.push_back(base + n + countTemp * circlePoints);
							trunkUVs.push_back(glm::vec2((float)n / circlePoints, (float)y / (uvYMax) * textureTrunkHeight));
							nSolution++;
						}
						else {
							trunkIndices.push_back(base + n + countTemp * circlePoints); trunkIndices.push_back(base + n + 1 + countTemp * circlePoints); trunkIndices.push_back(set + n + 1 + countTemp * circlePoints);
							trunkIndices.push_back(set + n + 1 + countTemp * circlePoints); trunkIndices.push_back(set + n + countTemp * circlePoints); trunkIndices.push_back(base + n + countTemp * circlePoints);
						}
					}
					else if (n == circlePoints - 1) {
						trunkIndices.push_back(base + n + count * circlePoints); trunkIndices.push_back(base + count * circlePoints); trunkIndices.push_back(set + count * circlePoints);
						trunkIndices.push_back(set + count * circlePoints); trunkIndices.push_back(set + n + count * circlePoints);  trunkIndices.push_back(base + n + count * circlePoints);
						trunkUVs.push_back(glm::vec2((float)n / circlePoints, (float)y / (uvYMax)));
						nSolution++;
					}
					else {
						//WORKS
						trunkIndices.push_back(base + n + count * circlePoints); trunkIndices.push_back(base + n + 1 + count * circlePoints); trunkIndices.push_back(set + n + 1 + count * circlePoints);
						trunkIndices.push_back(set + n + 1 + count * circlePoints); trunkIndices.push_back(set + n + count * circlePoints);  trunkIndices.push_back(base + n + count * circlePoints);
					}
					trunkUVs.push_back(glm::vec2((float)(n + nSolution) / circlePoints, (float)y / (uvYMax)));
				}
				for (int n = 0; n < circlePoints + 1; n++) {
					trunkUVs.push_back(glm::vec2((float)(n) / circlePoints, (float)(y+1) / (uvYMax)));
				}
				if (count > 0 && y == 0) {
					yMax = yMax + 1;
					ySoluton = -1;
				}
			}

			count += 3;

			//make branch check
			if ((randomSeedValue * (count + 1)) % branchMod == 0) {
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
		
		//MOVE PROCEDURE OF LEAF BRANCHES TO LEAFBRANCH CLASS AND LEAVES TO THE LEAF CLASS
		//CONDENSE ALGORITHM SIZE
		float f = 2;

		while (lineMax % 3 != 0) lineMax--;

		float r2 = 0.000176135 * sqrt(abs(-88063572 + 50843527 * trunkDiameter * lineSegments)) ;
		float r1 = sqrt(3) * sqrt(abs(2 - r2 * r2)) * f;

		while (lineHeight < lineMax - 2){
			//branch
			for (int n = 0; n < leafPoints ; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter))) * jagednessFactor / (trunkDiameter);
				leafVertices.push_back(glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations  * n)) , lineHeight, tempTrunkDiameter *  cos(glm::radians(itterations  * n))));
			}
		//place leaves parralel following angle
			//place approproate leaves parralel following angle
			for (int n = 0; n < leafPoints; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / trunkDiameter;
				if (count % 2 == 0)
					leafVertices.push_back(glm::vec3(r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , 0.0f));
				else
					leafVertices.push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2));
			}
			for (int n = 0; n < leafPoints; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / trunkDiameter;
				if (count % 2 == 0)
					leafVertices.push_back(glm::vec3(-r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , 0.0f));
				else
					leafVertices.push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, -r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2));

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
				leafVertices.push_back(glm::vec3(tempTrunkDiameter * sin(glm::radians(itterations  * n)) , lineHeight + 0, tempTrunkDiameter *  cos(glm::radians(itterations  * n))));

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
					leafVertices.push_back(glm::vec3(r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , 0));
				else
					leafVertices.push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, r2 * cos(glm::radians(itterations  * n)) + tempTrunkDiameter / 2.0 + r2 ));

			}
			for (int n = 0; n < leafPoints; n++) {
				int sign = -1;
				int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
				if (jagednessRandom % 2 == 0) sign *= -1;
				float tempTrunkDiameter = r1 + sign * (jagednessRandom) % ((int)(ceil(r1))) * jagednessFactor_Leaf / (trunkDiameter* y);
				if (count % 2 == 0)
					leafVertices.push_back(glm::vec3(-r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2 , lineHeight + r1 *  sin(glm::radians(itterations  * n)) , 0));
				else
					leafVertices.push_back(glm::vec3(0, lineHeight + r1 *  sin(glm::radians(itterations  * n)) + 0, -r2 * cos(glm::radians(itterations  * n)) - tempTrunkDiameter / 2.0 - r2));

			}
			lineHeight += lineSegments * y * 0.8;
			count++;
			//REPEAT 2X
		}
		int len = (float)(lineMax - 2) / lineSegments + 3;
		for (int i = 0; i < len; i++) {
			//branch
			int base = i * leafPoints * 3 + baseVerticesSize;
			int set = (i + 1)*leafPoints * 3 + baseVerticesSize;
			if (i != len - 1) {
				int nSolution = 0;
				for (int n = 0; n < leafPoints; n++) {
					if (n == leafPoints - 1) {
						leafIndices.push_back(base + n); leafIndices.push_back(base + 0); leafIndices.push_back(set + 0);
						leafIndices.push_back(set + 0);  leafIndices.push_back(set + n); leafIndices.push_back(base + n);
						////printf("%f %f\n", (float)(n + nSolution) / leafPoints, (float)i / (len) );
						leafUVs.push_back(glm::vec2((float)(n + nSolution) / leafPoints, (float)i / (len)));
						nSolution++;
					}
					else {
						leafIndices.push_back(base + n); leafIndices.push_back(base + n + 1); leafIndices.push_back(set + n + 1);
						leafIndices.push_back(set + n + 1);  leafIndices.push_back(set + n); leafIndices.push_back(base + n);
					}
					////printf("%f %f\n", (float)(n + nSolution) / leafPoints, (float)i / (len) );
					leafUVs.push_back(glm::vec2((float)(n + nSolution) / leafPoints, (float)i / (len)));
				}
				for (int n = 0; n < leafPoints + 1; n++) {
					////printf("%f %f\n", (float)(n) / leafPoints, (float)(i+1) / (len) );
					leafUVs.push_back(glm::vec2((float)(n + nSolution) / leafPoints, (float)(i+1) / (len)));
				}
			}
			else {
				for (int n = 0; n < leafPoints - 2; n++) {
					int base = i * leafPoints * 3 + baseVerticesSize;
					if (n == leafPoints - 2 - 1) {
						leafIndices.push_back(base+0); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2);  leafIndices.push_back(base - n); leafIndices.push_back(base+0);
					}
					else {
						leafIndices.push_back(base+0); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2);  leafIndices.push_back(base + 3); leafIndices.push_back(base+0);
					}
					if (n == leafPoints - 2 - 1) {
						leafIndices.push_back(base+0); leafIndices.push_back(base - n); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base+0);
					}
					//topsegment cover
					else {
						leafIndices.push_back(base+0); leafIndices.push_back(base + 3); leafIndices.push_back(base + 2);
					}
				}
				leafUVs.push_back({0.9f, 0.9f});leafUVs.push_back({1.0, 0.95});
				leafUVs.push_back({0.5f, 1.0f});
				leafUVs.push_back({0.0f, 0.95f});leafUVs.push_back({0.1f, 0.9f});
			}
			//leaf1&2 N and S side
			for (int j = 0; j < 2; j++) {
				for (int n = 0; n < leafPoints - 2; n++) {
					int base = leafPoints * (j + 1) + i * leafPoints * 3 + baseVerticesSize + n;
					//NORTH
					if (n == leafPoints - 2 - 1) {
						leafIndices.push_back(base+0); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2);  leafIndices.push_back(base - n); leafIndices.push_back(base+0);
					}
					else {
						leafIndices.push_back(base+0); leafIndices.push_back(base+0 + 1); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2);  leafIndices.push_back(base + 3); leafIndices.push_back(base+0);
					}
					//SOUTH
					if (n == leafPoints - 2 - 1) {
						leafIndices.push_back(base+0); leafIndices.push_back(base - n); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base+0);
					}
					else {
						leafIndices.push_back(base+0); leafIndices.push_back(base + 3); leafIndices.push_back(base + 2);
						leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base+0);
					}
				}
				leafUVs.push_back({0.9f, 0.9f});leafUVs.push_back({1.0, 0.95});
				leafUVs.push_back({0.5f, 1.0f});
				leafUVs.push_back({0.0f, 0.95f});leafUVs.push_back({0.1f, 0.9f});
			}
		}

		//Cap with a leaf
		for (int n = 0; n < leafPoints ; n++) {
			int sign = -1;
			int jagednessRandom = randomSeedValue  * (((int)(n * 13.4) % 17) + 1);
			if (jagednessRandom % 2 == 0) sign *= -1;
			float tempTrunkDiameter = trunkDiameter + sign * (jagednessRandom) % ((int)(ceil(trunkDiameter))) * jagednessFactor_Leaf / (trunkDiameter);
			leafVertices.push_back(glm::vec3(r1 * sin(glm::radians(itterations  * n)) , r2 *  cos(glm::radians(itterations  * n)) + lineHeight + r2, 0.0f));
			leafColor.push_back(brown);
		}
		for (int n = 0; n < leafPoints - 2; n++) {
			int base = leafVertices.size() - leafPoints + n;
			if (n == leafPoints - 2 - 1) {
				leafIndices.push_back(base+0); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
				leafIndices.push_back(base + 2);  leafIndices.push_back(base - n); leafIndices.push_back(base+0);
			}
			else {
				leafIndices.push_back(base+0); leafIndices.push_back(base + 1); leafIndices.push_back(base + 2);
				leafIndices.push_back(base + 2);  leafIndices.push_back(base + 3); leafIndices.push_back(base+0);
			}
			if (n == leafPoints - 2 - 1) {
				leafIndices.push_back(base+0); leafIndices.push_back(base - n); leafIndices.push_back(base + 2);
				leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base+0);
			}
			else {
				leafIndices.push_back(base+0); leafIndices.push_back(base + 3); leafIndices.push_back(base + 2);
				leafIndices.push_back(base + 2); leafIndices.push_back(base + 1); leafIndices.push_back(base+0);
			}
		}
		leafUVs.push_back({0.9f, 0.9f});leafUVs.push_back({1.0, 0.95});
		leafUVs.push_back({0.5f, 1.0f});
		leafUVs.push_back({0.0f, 0.95f});leafUVs.push_back({0.1f, 0.9f});

		leafUVs.push_back({0.9f, 0.9f});leafUVs.push_back({1.0, 0.95});
		leafUVs.push_back({0.5f, 1.0f});
		leafUVs.push_back({0.0f, 0.95f});leafUVs.push_back({0.1f, 0.9f});
		//conect branches
		//conect leaves

		//NOTE: LEAVE BRANCHES COME IN PAIRS
		countL++;
		//if (//Test)//("LEAF COUNT: %d\n", countL);
	}

    int limiter = 2;

    void initiateMove(int pastRot, AttatchmentGroupings* ag){
        glm::mat4 rotation;
        int circularPoints = circlePoints;
		int rotationPoint = abs((ag->angleY) % (circularPoints / limiter ));

        float r = 360.0/circularPoints  * (rotationPoint);
        int start = ag->start + 1;
        int max = ag->end + 1;
        for (int k = start; k < max; k++) {
			trunkVertices.at(k)  = makeRotations(glm::radians((float)ag->angleX), glm::radians(r),glm::radians((float)ag->angleZ),
						  trunkVertices.at(k));
        }
		int previousRotation = rotationPoint;
        moveSegments(previousRotation, ag);
        return;
    }
	void moveSegments(int previousRotation, AttatchmentGroupings* ag) {
		for (int m = 0; m < 2; m++) {
			if (ag->ag[m] == nullptr) continue;

			int moveTo = 0;
			int moveFrom = 0;

			int circularPoints = ag->ag[m]->type == 'L' ? leafPoints : circlePoints;
			int rotationPoint = abs((ag->ag[m]->angleY) % (circularPoints / limiter ));
			//printf("RP%d PR%d %d %% %d\n", rotationPoint, previousRotation,ag->ag[m]->angleY, circularPoints / limiter);
			if (ag->ag[m]->side == 'L') {
				moveTo = (ag->end - circularPoints + 1) + (( 0 + rotationPoint) % circularPoints);
				moveFrom = (ag->ag[m]->start + 1)  + ((0+ previousRotation) % circularPoints);

			}
			else {
				moveTo = (ag->end - circularPoints + 1) + (2 + rotationPoint ) % circularPoints;
                moveFrom = (ag->ag[m]->start + 1) + (2 + previousRotation) % circularPoints;
			}

			float r = 360.0/circularPoints  * (rotationPoint);

            int xMiror =  1;//(rotationPoint + previousRotation) % circularPoints == 3 ? 1: 1;
            int zMirror = 1;//(rotationPoint + previousRotation) % circularPoints == 2 ? 1 : 1;
			if (ag->ag[m]->type == 'B') {
				int start = ag->ag[m]->start + 1;
				int max = ag->ag[m]->end + 1;
                ////("%d, %d\n",start, max);
                for (int k = start; k < max; k++) {
					trunkVertices.at(k) = makeRotations(xMiror * glm::radians((float)ag->ag[m]->angleX), glm::radians(r),
                                                        zMirror * glm::radians((float)ag->ag[m]->angleZ), trunkVertices.at(k));
				}
			}
			else {
				int start = ag->ag[m]->start + 1;
				int max = ag->ag[m]->end + 1;
				for (int k = start; k < max; k++) {
					leafVertices.at(k) = makeRotations(xMiror * glm::radians((float)ag->ag[m]->angleX), glm::radians(r),
                                                       zMirror * glm::radians((float)ag->ag[m]->angleZ),	  leafVertices.at(k));
				}
			}

			//translate components onto branch(destination - position)
			std::vector<glm::vec3>* vContainer2 = nullptr;
			if (ag->ag[m]->type == 'B'){
				vContainer2 = &trunkVertices;
			}
			else{
				vContainer2 = &leafVertices;
			}
			//place on position
			std::vector<glm::vec3>* vContainer1 = nullptr;
			std::vector<GLuint>* passThroughInd = nullptr;
			if (ag->type == 'B'){
				vContainer1 = &trunkVertices;
				passThroughInd = &trunkIndices;
			}
			else{
				vContainer1 = &leafVertices;
				passThroughInd = &leafIndices;
			}
			////("%d %d %c\n", previousRotation, moveTo, ag->type);

			glm::vec3 translation = vContainer1->at(moveTo) - vContainer2->at(moveFrom);
			glm::vec3 boost = boostSegment(ag, ag->ag[m], vContainer2) * (float) heightChunking ;
				int start = ag->ag[m]->start + 1;
				int max = ag->ag[m]->end + 1;
				for (int k = start; k < max; k++) {
					vContainer2->at(k) += translation + boost;
				}
		connectSegments(ag, m,rotationPoint, previousRotation, circularPoints, passThroughInd);
		moveSegments(rotationPoint , ag->ag[m]);
	}
		return;

	}
	glm::vec3 boostSegment(AttatchmentGroupings* agLow, AttatchmentGroupings* agHigh, std::vector<glm::vec3>* vPntr){
		glm::vec3 AB = vPntr->at(agLow->end) - vPntr->at(agLow->end -1);
		glm::vec3 AC = vPntr->at(agLow->end-2) - vPntr->at(agLow->end -1);
		glm::vec3 low =  glm::normalize(glm::cross(AB, AC));
		glm::vec3 KJ = vPntr->at(agHigh->start + 1) - vPntr->at(agHigh->start+2);
		glm::vec3 KL = vPntr->at(agHigh->start + 3) - vPntr->at(agHigh->start+2);
		glm::vec3 high =  -glm::normalize(glm::cross(KJ, KL));
return low + high;
	}
	//TODO: COMPLETE CONECTORS
	void connectSegments(AttatchmentGroupings* ag, int m, int rotPoint, int prevPoint, int circularPoints, std::vector<GLuint>* indPntr){
		depth--;
		//go through each point on upper part of at depth circle
		if(ag->ag[m]->type == 'B') {
			int set = abs(circularPoints - rotPoint + prevPoint) ;
			////printf("%d\n", set);
			for (int j = 0; j < circularPoints; j++) {
				GLuint A = (ag->end - circularPoints + 1) + (j) % circularPoints;
				GLuint B = (ag->end - circularPoints + 1) + (1 + j) % circularPoints;
				GLuint C;
				indPntr->push_back(A);
				indPntr->push_back(B);
	//			if(ag->type == 'B') {
				C = (ag->ag[m]->start + 1) + (1 + j + set) % circularPoints;
	//			}
	//			else{
	//				C = (ag->ag[m]->start + 1) + (1 + j + set) % circularPoints;
	//			}
				indPntr->push_back(C);

				GLuint D = C;
				GLuint E;
	//			if(ag->ag[m]->type == 'B') {
				E = (ag->ag[m]->start + 1) + (j + set) % circularPoints;
	//			}
				GLuint F = A;
				indPntr->push_back(D);
				indPntr->push_back(E);
				indPntr->push_back(F);
				if (ag->ag[m]->type == 'B') {
					trunkUVs.push_back({(float) j / circularPoints, 0.0});
					trunkUVs.push_back({(float) (j + 1) / circularPoints, 0.0});
					trunkUVs.push_back({(float) (j + 1) / circularPoints, textureTrunkHeight + textureLeafEnd});
					trunkUVs.push_back({(float) (j) / circularPoints, textureTrunkHeight + textureLeafEnd});
				}
				else {
					leafUVs.push_back({(float) j / circularPoints, 0.0});
					leafUVs.push_back({(float) (j + 1) / circularPoints, 0.0});
					leafUVs.push_back({(float) (j + 1) / circularPoints, textureTrunkHeight + textureLeafEnd});
					leafUVs.push_back({(float) (j) / circularPoints, textureTrunkHeight + textureLeafEnd});
				}
			}
		}
	}
	//TODO
	void computeNormal() {

	}

	//values in radians
	glm::vec3 makeRotations(float xRot, float yRot, float zRot, glm::vec3 vector){
		//ROTATION ABOUT X
		// << xRot << " "<< yRot << " " << zRot <<"\n" << vector.x <<" " << vector.y << " " << vector.z << "\n";
		vector = glm::vec3(
				vector.x
				, vector.y * cos(xRot) - vector.z * sin(xRot)
				,	vector.y * sin(xRot) + vector.z * cos(xRot));
		//ROTATION ABOUT Z
		// << xRot << " "<< yRot << " " << zRot <<"\n" << vector.x <<" " << vector.y << " " << vector.z << "\n";
		vector = glm::vec3(
				vector.x * cos(zRot) - vector.y * sin(zRot)
				, vector.x * sin(zRot) + vector.y * cos(zRot)
				,	vector.z );
		//ROTATION ABOUT Y
		// << xRot << " "<< yRot << " " << zRot <<"\n" << vector.x <<" " << vector.y << " " << vector.z << "\n";
		vector = glm::vec3(
				vector.x * cos(yRot) + vector.z * sin(yRot)
				, vector.y
				,	-(vector.x * sin(yRot)) + vector.z * cos(yRot));
		// << xRot << " "<< yRot << " " << zRot <<"\n" << vector.x <<" " << vector.y << " " << vector.z << "\n-------\n";
	return vector;
	}

	void combineVectors() {
				//TEST: IS IT FASTER WITH WHILE LOOP OR SOMETHING ELSE?
		combinedVertices.reserve(trunkVertices.size() + leafVertices.size());
		combinedVertices.insert(combinedVertices.end(), trunkVertices.begin(), trunkVertices.end());
		combinedVertices.insert(combinedVertices.end(), leafVertices.begin(), leafVertices.end());

		//CONSIDER MERGING WITH FUNCTION BELOW?
		std::vector<GLuint> ind;
		unsigned int size = trunkVertices.size();
		unsigned int len = leafIndices.size();
		unsigned int i = 0;
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
		this->vao = initVertexArray( this->combinedVertices, this->combinedIndices, &vbo, &ebo);
		//stbi_image_free(image_data);
	}

	bool treeSetup(const GLuint& shader_program, float trunkDiameter, float seed){
		generateTreeA(0, trunkDiameter, seed, 0, 0, 0, 'C', nullptr, 0);
		computeNormal();
		//computeUV();
		combineVectors();
		bufferObject(shader_program);
		return true;
	}

public:
	const std::vector<glm::vec3>& getVertices()
	{
		return this->combinedVertices;
	}


	GLuint getVAO()
	{
		return this->vao;
	}

	const int getColorType(){return 0;};

#include <iostream>
#include <cstdio>
#include <ctime>

	TreeA(const GLuint& shader_program, Entity* entity, float trunkDiameter, float seed):
			DrawableEntity(shader_program, entity){
		std::clock_t startTime;
		double duration;

		startTime = std::clock();

		draw_mode = GL_TRIANGLES;

		if (trunkDiameter <= 0.0) trunkDiameter = 1.0;
		widthCut = 0.5;//trunkDiameter / 32;
		finalCut = widthCut;

		leafStartIndices.push_back({ -1, 0, 0, 0 });
		trunkStartIndices.push_back({ -1, 0, 0, 0 });

		//CONSIDERATION: MULTITHREAD THE GENERATION TO NOT INTERFERE WITH GAME LOOP
		treeLoaded = treeSetup(shader_program, trunkDiameter, seed);

        float globalRotation = treeRandom(trunkDiameter,seed,widthCut*10);

        rotate(globalRotation, glm::vec3(0.0f,1.0f,0.0f));

		duration = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
		printf("Duration %f Units: %f ms\n", trunkDiameter, duration*1000);
	};

};
//
//glm::vec3 U = leafVertices.at(base + n+1) - leafVertices.at(base + n);
//glm::vec3 V = leafVertices.at(set + n) - leafVertices.at(base + n);
//glm::vec3 normal = glm::normalize(glm::cross(U, V));
//combinedNormals.push_back(normal);

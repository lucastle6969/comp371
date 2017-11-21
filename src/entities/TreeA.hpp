/*
 * TODO:    ||  merge  || 			Textures               Tree Variations(no rotation double, regular, shrub patch[0 alpha lines])        Texture Variations(Oak, Birch, pine)
 *			6:00	`````````									00:00										Morning
 *			S		U		N		D	A		Y			---			            M				O					N				D				A					Y

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

#ifndef COMP371_TREE_A_HPP
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

#include <iostream>
#include <cstdio>
#include <ctime>

#include "../TreeRandom.hpp"
#include "TrunkA.hpp"
#include "LeafContainerA.hpp"

#include "Entity.hpp"
#include "DrawableEntity.hpp"
#include "Tree.hpp"

class TreeA : public Tree {
private:

	//CONSIDERATION FOR MULTITHREADED LOADING
	bool treeLoaded = false;

	const int branches = 1;
	const int k = 250;
	float limiter = 1;
	int previousRotationCap = 8;

float boostFactor = 0.5;
float heightChunking = 20;//INVERSE

	const int minYBranchAngle = 20;
	const int maxYBranchAngle = 45;
	const int minYTrunkAngle = 0;
	const int maxYTrunkAngle = 20;

	double trunkRatio = 1.0;
	double branchRatio = 0.850;

	inline float ShootCalculation(float trunkDiameter, double ratio){	return pow(pow(trunkDiameter, 2) / (branches + 1), 1.0 / 2.0) * ratio;	}
	inline int lineMAX(float trunkDiameter) { return ceil(pow(pow(trunkDiameter, 2) * k, 1.0 / 2.0)); }

	void generateTreeA(int _case, float trunkDiameter, float seed, float angleX, float angleY, float angleZ,
                       char tag, AttatchmentGroupings* ag, float lineHeight);

	float trunk(float trunkDiameter, float seed, float lineHeight);
	void leafBranch(float trunkDiameter, float seed, float lineHeight);

	//TODO: ADD SHADER: DARKER ON MORE Z USING TEXTURES IN SHADER
	//PUT TEXTURE LOADING IN SEPERATE CLASS. MAKE IT ONLY CALLED ONCE FOR THE FIRST TREE LOADED.
	void bufferObject(const GLuint& shader_program);

	void initiateMove(int pastRot, AttatchmentGroupings* ag);

	void moveSegments(int previousRotation, AttatchmentGroupings* ag);

	bool treeSetup(const GLuint& shader_program, float trunkDiameter, float seed);

public:

	TreeA(const GLuint& shader_program, Entity* entity, double trunkDiameter, int seed):
			Tree(heightChunking, boostFactor, shader_program, entity, 'A'){
		std::clock_t startTime;
		double duration;
		startTime = std::clock();

		//CONSIDERATION: MULTITHREAD THE GENERATION TO NOT INTERFERE WITH GAME LOOP
		treeLoaded = treeSetup(shader_program, trunkDiameter, seed);

		float globalRotation = TreeRandom::treeRandom(trunkDiameter,seed,widthCut*10);
		rotate(globalRotation, glm::vec3(0.0f,1.0f,0.0f));


		duration = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
		printf("Duration of A %f Units: %f ms\n", trunkDiameter, duration*1000);
	};
};
#endif //treeA
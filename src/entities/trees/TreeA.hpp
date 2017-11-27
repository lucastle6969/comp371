/*
# DOCUMENTATION OF TREE GENERATION

ONE CENTRAL .CPP FILE CONTROLS GENERATION OF TREE STRUCTURE. THIS IS REFERRED TO AS TREEA, TREEB, OR TREEC ALL STEMMING FROM THE SUPERCLASS TREE.
'TreeA(const GLuint& shader_program, float trunkDiameter, float seed)'
THE TREES ARE ALL BROKEN DOWN INTO A TRUNK COMPONENT AND A LEAF CONTAINER COMPONENT(BRANCH AND LEAF) THAT ARE MESHED TOGETHER USING A RECURSIVE ALGORITHM AND STORED COMBINED VERTICES THROUGH POINTERS.
LEAVES ARE DIAMONDS, TRUNKS ARE SQUARES. TRUNKS ROTATE ON THE POINTS OF OTHER TRUNKS.
CALL TO GENERATE OBJECT WILL RESULT IN AN OBJECT WITH A STORED VAO IN THESE PARAMETERS.
CREATES TRUNK AND LEAF VECTORS+INDICES WHICH ARE MERGED TOGETHER ARE SENT TO THE GPU.

THE TREE TYPES ARE AS FOLLOWS:
1) TREEA: THIS IS A CONVENTIONAL TALL HARDWOOD TREE WITH BRANCHES THAT ARE MADE TO STRAIGHTEN OUT THE FURTHER FROM THE BASE TRUNK IT GETS. THE ARE STACKED ON THE INSIDE OF THE TRUNK.
2) TREEB: THE IS A BUSH TREE WITH LOTS OF INWARD TURNING AND ARE STACKED ON THE OUTSIDE OF A TRUNK.
3) TREEC: THIS IS A CLUSTER TYPE. THE TREEC CREATES ONE CENTRAL HEAD TREE WHICH IS AROUND THE HEIGHT ON THE PLAYER AND SHOOTS OFF SMALLER SPROUTS TO GIVE 3D GROUND COVER.

CURRENT LOADING SPEED SUGGESTS THE 4-8 DIAMETER RANGE WILL GENERATE QUICKLY. SOME FRAME LOSS IS INEVITABLE WHEN CREATING TREES.

### THE PROGRAM
PROGRAM ENTRY POINT IN IN THE CONSTRUCTOR FILE WHERE IT INITIALIZES SUBCLASSES AND SETS SOME BASE PARAMETERS.
AFTER INITIALIZED THE generateTreeA FUNCTION STARTS TO BUILD THE STARTER TRUNK,
THEN ENTERS BINARY RECURSIONS OF ITSELF TO GENERATE THE TRUNKS AND LEAVES WHEREUPON REACHING THE BASE CASE AFTER CONSTRUCTING A LEAF '                  generateTreeA(END_TRUNK, trunkDiameter, seed, angleX, angleY, angleZ, tag, ag, lineHeight);' IT MOVES ONTO THE NEXT BRANCH. AFTER COMPLETING RECURSIONS SEGMENTS OF ARE ROATED AND THEN MOVED ON TOP OF EACH OTHER WITH TOP MOST VERTICES OF THE LOWER TOUCHING AN APPROPRIATE HIGHEST VERTICES ON THE LOWEST.
SEGMENTS ARE STORED INTO ATTACHMENT GROUPINGS AS A LINKED LIST AND ARE STITCHED TOGETHER IN THE MOVE PROCESS USING THE ANGLES COMPUTED DURING RECURSIONS
FINALY THE TREE IS GIVEN A SEED BASED RANDOM ROTATION.
TREEA CAN HAVE TWO TEXTURES. AUTUMN OR SUMMER.
ALL TREES HAVE AN "ALIEN" STATE USING A POSITION BASED COLORING AND A REGULAR TEXTURE.

### COMPONENT PROPERTIES
TRUNKS HAVE FOUR PROPERTIES.
1) THE WIDTH OF THE SEGMENT
2) THE JADEDNESS(BUMPINESS) OF THE SEGMENT
3) THE MAX HEIGHT IT CAN REACH
4) THE LEVEL THAT SEGMENTS ARE SEPARATED
5) THE ANGLE BRANCHES CAN HAVE
6) THE SEED RELATED TO IT
THEY ARE STRAIGHT LINE PIECES OF ROUGHLY CIRCULAR SIZE

LEAF BRANCHES HAVE FOUR PROPERTIES
1) THE WIDTH OF THE BRANCH SEGMENT
2) THE JADEDNESS(BUMPINESS) OF THE SEGMENT'S LEAVES
3) THE MAX HEIGHT IT CAN REACH
4) THE SEED RELATED TO IT
AT EVERY PART OF THE BRANCH IT SPLITS  INTO TWO LEAVES ALTERNATING IN PERPENDICULARS.
AT THE LAST 2 PARTS THE LEAVES GET SMALLER UNTIL THE LAST WHERE IT ENDS WITH A LEAF.
LEAFS ARE DOUBLE SIDED FOR BACKFACE CULLING.

### ELEMENTS
IN ORDER TO GENERATE PROPER TEXTURES THE LEAVES AND TRUNKS ALL USE DIFFERENT SEGMENTS OF ONE TEXTURE MAP. THE DECISION OF WHAT IS USED WHEN RESTS ON THE UV CREATION ALGORITHM.
NORMALS ARE COMPUTED PER VERTEX WITH NO GENERALIZATIONS. MAKING APPROXIMATIONS WOULD INVALIDATE THE VALUE OF USING BUMPINESS ON THE TRUNKS.
INDICES ARE CONSTRUCTED USING LOCAL OBJECT VERTICES AS WELL AS THE LINKED LIST'S START AND ENDING VERTICES NUMBERS TO CREATE CONNECTORS.
TEXTURES ARE CREATED WITH A BINARY SWITCH THAT DETERMINS IF THE TEXTURE UV'S V COMPONENT SHOULD SEQUENCE 101 OR 010.
*/

#ifndef COMP371_TREE_A_HPP
#define COMP371_TREE_A_HPP

#define START_TRUNK 0
#define TRUNK 2
#define LEAF 3
#define END_TRUNK -1


#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/OpenGL.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <iostream>
#include <cstdio>
#include <ctime>

#include "src/TreeRandom.hpp"
#include "TrunkAB.hpp"
#include "LeafContainerAB.hpp"

#include "src/entities/Entity.hpp"
#include "src/entities/DrawableEntity.hpp"
#include "Tree.hpp"

class TreeA : public Tree {
private:
    static constexpr int branches = 1;
    static constexpr int k = 250;
	static constexpr float widthCut = 0.5;
	static constexpr int previousRotationCap = 8;

    static constexpr float boostFactor = 0.25;
    static constexpr int heightChunking = 20;//INVERSE

    static constexpr int minYBranchAngle = 30;
	static constexpr int maxYBranchAngle = 45;
	static constexpr int minYTrunkAngle = 0;
	static constexpr int maxYTrunkAngle = 20;

	static constexpr double trunkRatio = 1.0;
	static constexpr double branchRatio = 0.850;

	float trunk(float trunkDiameter, const float& seed, float lineHeight);
	void leafBranch(float trunkDiameter, const float& seed, float lineHeight);

	void initiateMove(AttatchmentGroupings* ag);

	void moveSegments(const int& previousRotation, AttatchmentGroupings* ag);

	void treeSetup(const GLuint& shader_program, float trunkDiameter, const int&);

	void generateTreeA(const int& _case, float trunkDiameter, const float& seed,
					   float angleX, float angleY, float angleZ,
					   char tag, AttatchmentGroupings* ag, float lineHeight);

	//PUT TEXTURE LOADING IN SEPERATE CLASS. MAKE IT ONLY CALLED ONCE FOR THE FIRST TREE LOADED.
	void bufferObject(const GLuint& shader_program);

protected:

	void createSegmentNormals(AttatchmentGroupings* ag,int m);

	void createConnectorElements(AttatchmentGroupings* ag,int m);

	float limiter = 1;

    std::string textureMap;

    const std::string textureMap1 = "../textures/TreeATexture.jpg";
    const std::string textureMap2 = "../textures/TreeATexture - Alternative.jpg";
public:

	TreeA(const GLuint& shader_program, Entity* entity, float trunkDiameter, const int& seed);

	GLuint getTextureId();
	const int getColorType();
};
#endif //treeA
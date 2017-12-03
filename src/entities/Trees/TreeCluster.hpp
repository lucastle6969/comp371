////////DISCONTINUED////////////

#ifndef COMP371_TREE_C_HPP
#define COMP371_TREE_C_HPP

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


#include "../DrawableEntity.hpp"
#include "../Entity.hpp"
#include "Tree.hpp"
#include "TreeClusterItem.hpp"
#include "src/TreeRandom.hpp"

class TreeCluster{
private:
    static constexpr float sizeVariation = 1.2;
    static constexpr int heightChunking = 0;
    static constexpr int boostFactor = 0;
    static int spacingConstant;
    static int maxWidth(const float& trunkDiameter);

public:
    static void generateCluster(
	    std::vector<Tree*>* const& treeContainer,
	    Entity* const& parentEntity,
	    long seed,
	    const int& numberOfTrees,
		const GLuint& shader_program,
	    const float& trunkDiameter,
	    const bool& isAlien,
	    const float& magnitude
    );
    static void setSpacingConstant(int k);
};
#endif

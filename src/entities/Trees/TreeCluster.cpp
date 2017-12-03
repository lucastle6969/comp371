#ifndef COMP371_TREE_C_cPP
#define COMP371_TREE_C_cPP

#include <src/loadTexture.hpp>
#include <src/entities/World.hpp>
#include "TreeCluster.hpp"

int TreeCluster::spacingConstant = 5;

int TreeCluster::maxWidth(const float &trunkDiameter) {
	return (int) (pow(spacingConstant * (trunkDiameter + 1), 1.0 / 2) + trunkDiameter);
}

void TreeCluster::generateCluster(
	std::vector<Tree*>* const& treeContainer,
	Entity* const& parentEntity,
	long seed,
	const int& numberOfTrees,
	const GLuint& shader_program,
	const float& trunkDiameter,
	const bool& isAlien,
	const float& magnitude
) {
	//center piece
	auto* tci = new TreeClusterItem(shader_program, parentEntity, trunkDiameter, seed, isAlien);
	tci->setLocationWithPoints(0, 0);

	treeContainer->emplace_back(tci);

	//distribute in random cirlce
	float tempTrunkDiameter;
	int i = 0;
	for (; i < numberOfTrees; i++) {
		if (sizeVariation != 0) {
			tempTrunkDiameter = TreeRandom::middleSquareRange(seed, trunkDiameter * 2,
			                                                  sizeVariation) / 20.0f;
			seed = (unsigned long) (pow(seed, 2) * (tempTrunkDiameter + 1)) % 99991;
		} else tempTrunkDiameter = trunkDiameter / 1.0f;
		seed = seed == 0 ? (tempTrunkDiameter + 1) * 913 * magnitude : seed;

		float distanceFromCenter = TreeRandom::middleSquareRange(seed,
		                                                         maxWidth(trunkDiameter),
		                                                         1.0);
		distanceFromCenter =
				distanceFromCenter <= 1 ? TreeRandom::middleSquareRange(seed * 2,
				                                                        maxWidth(
						                                                        trunkDiameter) *
				                                                        1.00, 1.0)
				                        : distanceFromCenter;
		float circleAngle = TreeRandom::middleSquareRange(seed, 360.0, 0.0) *
		                    TreeRandom::treeOddEvenRandom(distanceFromCenter, seed,
		                                                  circleAngle);

		float distScale = .125f;
		float xPos = cos(glm::radians(circleAngle)) * distanceFromCenter * distScale;
		float zPos = sin(glm::radians(circleAngle)) * distanceFromCenter * distScale;

		auto* tci = new TreeClusterItem(shader_program, parentEntity,
		                                tempTrunkDiameter <= 0 ? 0 : tempTrunkDiameter,
		                                seed, isAlien);

		tci->setLocationWithPoints(xPos, zPos);
		treeContainer->emplace_back(tci);
	}
}

void TreeCluster::setSpacingConstant(int k) {
	TreeCluster::spacingConstant = k;
}


#endif

//
// Created by Tof on 2017-11-18.
//

#ifndef PROCEDURALWORLD_TREERANDOM_H
#define PROCEDURALWORLD_TREERANDOM_H

class TreeRandom {
public:
    static int k;

    static int treeRandom(float trunkDiameter, float seed, float lineHeight) {
        int randomSeedValue = ((int)(trunkDiameter * seed * (((int)lineHeight % 10 * (int)(lineHeight*7) % 100 + 1 ) * 10) * 3)) % (k*10 * (int)ceil(trunkDiameter));
        if (randomSeedValue % 2 == 0) randomSeedValue = -(randomSeedValue + 1);
        return randomSeedValue;
    }
    static int treeOddEvenRandom(float trunkDiameter, float seed, float lineHeight) {
        int randomSeedValue = ((int)(trunkDiameter * seed * (((int)lineHeight % 10 * (int)(lineHeight*7) % 100 + 1 ) * 10) * 3)) % (k*10 * (int)ceil(trunkDiameter));
        return randomSeedValue;
    }
    static int branchAngleFromRandom(float trunkDiameter, float seed, float lineHeight, int maxYBranchAngle , int minYBranchAngle) {return treeRandom(trunkDiameter, seed, lineHeight) % (maxYBranchAngle - minYBranchAngle) + minYBranchAngle;}
    static int trunkAngleFromRandom(float trunkDiameter, float seed, float lineHeight, int maxYTrunkAngle ,int minYTrunkAngle) { return treeRandom(trunkDiameter, seed, lineHeight) % (maxYTrunkAngle - minYTrunkAngle) + minYTrunkAngle; }
};

     int TreeRandom::k = 777;

#endif //PROCEDURALWORLD_TREERANDOM_H

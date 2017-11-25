//
// Created by Scherezade on 25/11/2017.
//

#include "AttatchmentGroupings.hpp"

AttatchmentGroupings::AttatchmentGroupings(const int& start,	const int& end,
                     const int& angleX, const int& angleY, const int& angleZ,
                     const char& type,	const char& side){
    this->start = start;
    this->end = end;
    this->angleX = angleX;
    this->angleY = angleY;
    this->angleZ = angleZ;
    this->type = type;
    this->side = side;
    this->vHeight = vHeight;
    ag[0] = nullptr; ag[1] = nullptr;
}
AttatchmentGroupings::AttatchmentGroupings(const int& start,	const int& end,
                     const int& angleX, const int& angleY, const int& angleZ,
                     const char& type,	const char& side, const int& vHeight){
    this->start = start;
    this->end = end;
    this->angleX = angleX;
    this->angleY = angleY;
    this->angleZ = angleZ;
    this->type = type;
    this->side = side;
    this->vHeight = vHeight;
    ag[0] = nullptr; ag[1] = nullptr;
}
void AttatchmentGroupings::selfErase() {
    for (AttatchmentGroupings* agI: this->ag){
        if (agI != nullptr) {
            agI->selfErase();
            delete agI;
            agI = nullptr;
        }
    }
}
//
// Created by Scherezade on 25/11/2017.
//

#ifndef PROCEDURALWORLD_ATTATCHMENTGROUPINGS_HPP
#define PROCEDURALWORLD_ATTATCHMENTGROUPINGS_HPP

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
    int vHeight;

    AttatchmentGroupings* ag[2];

    AttatchmentGroupings(const int& start,	const int& end,
                         const int& angleX, const int& angleY, const int& angleZ,
                         const char& type,	const char& side);

    AttatchmentGroupings(const int& start,	const int& end,
                         const int& angleX, const int& angleY, const int& angleZ,
                         const char& type,	const char& side, const int& vHeight);
    void selfErase() ;
};


#endif //PROCEDURALWORLD_ATTATCHMENTGROUPINGS_HPP

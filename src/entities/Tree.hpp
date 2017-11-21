#ifndef PROCEDURALWORLD_TREE_HPP
#define PROCEDURALWORLD_TREE_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include "Entity.hpp"
#include "DrawableEntity.hpp"

class Tree : public DrawableEntity {
private:
	std::vector<glm::vec3> vertices;
	GLuint vao;
public:
	Tree(
		const GLuint& shader_program,
		const float& world_x_location,
		const float& world_z_location,
		const float& base_span
	) : Tree(
			shader_program,
			world_x_location,
			world_z_location,
			base_span,
			nullptr
	) {}
	Tree(
		const GLuint& shader_program,
		const float& world_x_location,
		const float& world_z_location,
		const float& base_span,
		Entity* parent
	);
	const std::vector<glm::vec3>& getVertices() override;
	GLuint getVAO() override;
	const int getColorType() override;
};


#endif //PROCEDURALWORLD_TREE_HPP


/*
//
// Created by Tof on 2017-11-19.
//

#ifndef PROCEDURALWORLD_TREE_H
#define PROCEDURALWORLD_TREE_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
//container class

#include "trunkA.hpp"
#include "LeafContainerA.hpp"
#include "entity.hpp"

class Tree {
private:
protected:
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

    AttatchmentGroupings(int start,	int end, int angleX, int angleY, int angleZ, char type,	char side){
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

    std::vector<AttatchmentGroupings> branchStore;
    std::vector<glm::vec3>* combinedVertices = new std::vector<glm::vec3>; 	std::vector<GLuint>* combinedIndices = new std::vector<GLuint>;		std::vector<glm::vec3>* combinedColor = new std::vector<glm::vec3>; std::vector<glm::vec3>* combinedNormals= new std::vector<glm::vec3>;
    std::vector<glm::vec2>* combinedUV = new std::vector<glm::vec2>; std::vector<std::vector<int>> *combinedStartIndices = new std::vector<std::vector<int>> ;
    GLuint vao; GLuint vbo; GLuint ebo; GLuint tbo;

    float heightChunking;
    int boostFactor;

    float widthCut;
    float finalCut;

    Entity* e;

public:
    Tree(int heightChunking, float boostFactor){
        this->heightChunking = heightChunking;
        this->boostFactor = boostFactor;
    }
    Tree(const GLuint& shader_program, Entity* entity, float trunkDiameter, float seed){};
    ~Tree(){
        delete combinedVertices; 	delete combinedIndices ;	delete combinedColor; delete combinedNormals;
        delete combinedUV ; delete combinedStartIndices;
    }

    glm::vec3 boostSegment(AttatchmentGroupings* agLow, AttatchmentGroupings* agHigh, std::vector<glm::vec3>* vPntr);

    //values in radians
    glm::vec3 makeRotations(float xRot, float yRot, float zRot, glm::vec3 vector);

    void connectSegments(AttatchmentGroupings* ag, int m, int rotPoint, int prevPoint, int circularPoints,
                         std::vector<GLuint>* indPntr);

    void computeElementsInitial(AttatchmentGroupings* ag);
};


#endif //PROCEDURALWORLD_TREE_H
*/
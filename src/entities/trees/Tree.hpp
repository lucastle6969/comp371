
#ifndef PROCEDURALWORLD_TREE_H
#define PROCEDURALWORLD_TREE_H

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/OpenGL.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "src/constants.hpp"
#include "TrunkAB.hpp"
#include "LeafContainerAB.hpp"
#include "src/entities/Entity.hpp"
#include "src/entities/DrawableEntity.hpp"
#include "AttatchmentGroupings.hpp"

//wrapper class
class Tree: public DrawableEntity {
private:
    char type;
protected:

    float shootCalculation(const float& trunkDiameter, const double& ratio,const int& branches);
    int lineMAX(const float& trunkDiameter, int k);

    std::vector<AttatchmentGroupings> branchStore;
    std::vector<glm::vec3>* combinedVertices = new std::vector<glm::vec3>;
    std::vector<GLuint>* combinedIndices = new std::vector<GLuint>;
    std::vector<glm::vec3>* combinedNormals= new std::vector<glm::vec3>;
    std::vector<glm::vec2>* combinedUV = new std::vector<glm::vec2>;
    std::vector<std::vector<int>> *combinedStartIndices = new std::vector<std::vector<int>> ;
    GLuint vao, vbo, ebo, nbo, UVbo;

    float heightChunking;
    int boostFactor;

    float widthCut;
    float finalCut;

    Entity* e;
    float trunkDiameter;
public:
        char getType(){return type;}

    glm::vec3 boostSegment(const AttatchmentGroupings* agLow, const AttatchmentGroupings* agHigh,
                           const std::vector<glm::vec3>* vPntr);

    //values in radians
    glm::vec3 makeRotations(const float& xRot, const float& yRot, const float& zRot, glm::vec3 vector);

    void connectSegments(AttatchmentGroupings* ag, const int& m, int k,
                         const int& rotPoint, const int& prevPoint, const int& circularPoints,
                         std::vector<GLuint>* indPntr);

    void computeElements(AttatchmentGroupings* ag);

    const std::vector<glm::vec3>& getVertices();

    GLuint getVAO();

    const int getColorType();

    Tree(int heightChunking, float boostFactor, const GLuint& shader_program, Entity* entity, const char& type);
    ~Tree();
};


#endif //PROCEDURALWORLD_TREE_H

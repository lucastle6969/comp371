#ifndef PACMAN3D_ENTITY_H
#define PACMAN3D_ENTITY_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

class Entity {
private:
	Entity* parent;
	GLuint* vao;
	glm::mat4 scale_matrix;
	glm::mat4 rotation_matrix;
	glm::mat4 translation_matrix;
	glm::mat4 model_matrix;
	void orient(const float& angle);

protected:
	std::vector<glm::vec3>* vertices;
	void initVertexArray(const GLuint& shader_program);

public:
	Entity() : Entity(nullptr) {}
	explicit Entity(Entity* parent);
	~Entity();
	std::vector<glm::vec3>* getVertices();
	GLuint* getVAO();
	virtual const int* getColorType();
	virtual const GLenum* getDrawMode();
	glm::mat4& getModelMatrix();
	void scale(const float& scalar);
	void moveUp(const int& units);
	void moveDown(const int& units);
	void moveLeft(const int& units);
	void moveRight(const int& units);
	void setPosition(const float& x, const float& y, const float&z = 0.0f);
};

#endif //PACMAN3D_ENTITY_H

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

// Abstract class

class Entity {
private:
	Entity* parent;
	glm::mat4 scale_matrix;
	glm::mat4 rotation_matrix;
	glm::mat4 translation_matrix;
	glm::mat4 model_matrix;
	bool hidden;
	void orient(const float& angle);

protected:
	GLenum draw_mode;
	static GLuint initVertexArray(const GLuint& shader_program, const std::vector<glm::vec3>& vertices);
	virtual const glm::mat4& getBaseRotation();

public:
	Entity() : Entity(nullptr) {}
	explicit Entity(Entity* parent);
	// pure virtual methods must be overridden by derived classes
	virtual const std::vector<glm::vec3>& getVertices() = 0;
	virtual GLuint getVAO() = 0;
	virtual const int getColorType() = 0;
	// end pure virtual functions
	GLenum getDrawMode();
	const glm::mat4& getModelMatrix();
	glm::vec3 getPosition();
	bool isHidden();
	void scale(const float& scalar);
	void rotate(const float& angle, const glm::vec3& axis);
	void resetRotation();
	void moveUp(const int& units = 1);
	void moveDown(const int& units = 1);
	void moveLeft(const int& units = 1);
	void moveRight(const int& units = 1);
	void setPosition(const float& x, const float& y, const float& z = 0.0f);
	void setDrawMode(const GLenum& draw_mode);
	void hide();
	void unhide();
};

#endif //PACMAN3D_ENTITY_H

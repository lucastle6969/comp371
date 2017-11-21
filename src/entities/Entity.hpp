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
	std::vector<Entity*> children;
	glm::mat4 scale_matrix;
	glm::mat4 rotation_matrix;
    glm::mat4 translation_matrix;
	glm::mat4 model_matrix;
	bool hidden;
	void orient(const glm::vec3& forward_vec);

protected:
	virtual const glm::mat4& getBaseScale();
	virtual const glm::mat4& getBaseRotation();
	virtual const glm::mat4& getBaseTranslation();
	virtual const glm::vec3& getDefaultFaceVector();
	// this method is NOT responsible for freeing memory
	// and does NOT remove the parent pointer from the child
	void detachChild(Entity* const& child);

public:
	Entity() : Entity(nullptr) {}
	explicit Entity(Entity* parent);
	virtual ~Entity() = default;
	const glm::mat4& getModelMatrix();
	glm::vec3 getPosition();
	glm::vec3 getScale();
	bool isHidden();
	virtual void scale(const float& scalar);
    virtual void rotate(const float& angle, const glm::vec3& axis);
    virtual void resetRotation();
    virtual void moveForward(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f);
    virtual void moveBack(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f);
    virtual void moveLeft(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f);
    virtual void moveRight(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f);
    virtual void translate(const glm::vec3& translation_vec);
    virtual void setPosition(const glm::vec3& position);
    virtual void hide();
    virtual void unhide();
    virtual void toggleHide();
	virtual void draw(const glm::mat4& view_matrix, const glm::mat4& projection_matrix);
};

#endif //PACMAN3D_ENTITY_H

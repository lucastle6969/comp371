#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <algorithm>

#include "Entity.hpp"
#include "../utils.hpp"

Entity::Entity(Entity* parent)
{
	// The parent entity's model matrix will be multiplied against this
	// entity's own transformation matrix when the model matrix is requested.
	// The parent will also call this entity's draw method each time its own
	// draw method is called.
	// If no parent is passed, then we'll ignore that field.

	this->parent = parent;
	if (parent) {
		parent->children.push_back(this);
	}

	this->opacity = 1.0f;

	// can be toggled with this->hide(), this->unhide().
	this->hidden = false;
}

const glm::mat4 Entity::getModelMatrix() const
{
	static glm::mat4 identity;

	// if we have a parent entity we want to adjust our transformation
	// to incorporate the context of the parent's transformation
	glm::mat4 parent_model_matrix = this->parent ? this->parent->getModelMatrix() : identity;

	return (parent_model_matrix *
			this->translation_matrix * this->getBaseTranslation() *
			this->rotation_matrix * this->getBaseRotation() *
			this->scale_matrix * this->getBaseScale());
}

glm::vec3 Entity::getPosition() const
{
	return utils::getTranslationVector(this->translation_matrix);
}

float Entity::getOpacity()
{
	return this->opacity;
}

glm::vec3 Entity::getScale() const
{
	return utils::getScaleVector(this->scale_matrix);
}

bool Entity::isHidden()
{
	// true either if this entity is explicitly hidden or its parent is hidden
	return this->hidden || (this->parent ? this->parent->isHidden() : false);
}

void Entity::scale(const float& scalar)
{
	this->scale_matrix = glm::scale(this->scale_matrix, glm::vec3(scalar));
}

void Entity::scale(const glm::vec3& scale_vector){
    this->scale_matrix = glm::scale(this->scale_matrix, scale_vector);
}

void Entity::rotate(const float& angle, const glm::vec3& axis)
{
	// rotation angle is in radians
	this->rotation_matrix = glm::rotate(this->rotation_matrix, angle, axis);
}

void Entity::resetRotation()
{
	static glm::mat4 identity;
	this->rotation_matrix = identity;
}

void Entity::translate(const glm::vec3& translation_vec) {
    this->translation_matrix = glm::translate(
            this->translation_matrix, translation_vec);
}

void Entity::move(const glm::vec3& move_vec, const float& units)
{
	this->translate(units * glm::normalize(move_vec));
	this->orient(move_vec);
}

void Entity::setPosition(const glm::vec3& position)
{
	static glm::mat4 identity;

	this->translation_matrix = glm::translate(identity, position);
}

void Entity::setOpacity(const float& opacity)
{
	this->opacity = glm::clamp(opacity, 0.0f, 1.0f);
}

void Entity::hide()
{
	this->hidden = true;
}

void Entity::unhide()
{
	this->hidden = false;
}

void Entity::toggleHide()
{
	this->hidden = !this->hidden;
}

void Entity::drawIfOpaque(
	const glm::mat4& view_matrix,
	const glm::mat4& projection_matrix,
	const Light& light
) {
	// draw opaque objects
	for (Entity* child : this->children) {
		child->drawIfOpaque(view_matrix, projection_matrix, light);
	}
}

void Entity::drawIfTransparent(
	const glm::mat4& view_matrix,
	const glm::mat4& projection_matrix,
	const Light& light
) {
	// draw transparent objects

	// We're iterating in reverse since we know the player entity will be first,
	// and the only currently known transparency issue is drawing the player before
	// other transparent objects which are closer to the camera.
	// TODO: Actually sort transparent objects? Though would be slower / maybe unnecessary
	for (size_t i = this->children.size(); i--; ) {
		this->children[i]->drawIfTransparent(view_matrix, projection_matrix, light);
	}
}

void Entity::orient(const glm::vec3& new_face_vec)
{
	static glm::mat4 identity;

	// re-write our rotation matrix to orient our model toward the given vector
	this->rotation_matrix = glm::rotate(
			identity,
			// compute the angle between the default face vector and our new face vector
			(float)acos(glm::dot(glm::normalize(this->getDefaultFaceVector()), glm::normalize(new_face_vec))),
			// compute the cross product as the rotation axis
			glm::cross(this->getDefaultFaceVector(), new_face_vec)
	);
}

// derived classes should override this if the model
// needs a fundamental scale offset
const glm::mat4& Entity::getBaseScale() const
{
	static glm::mat4 identity;
	return identity;
}

// derived classes should override this if the model
// needs a fundamental rotation offset
const glm::mat4& Entity::getBaseRotation() const
{
	static glm::mat4 identity;
	return identity;
}

// derived classes should override this if the model
// needs a fundamental translation offset
const glm::mat4& Entity::getBaseTranslation() const
{
	static glm::mat4 identity;
	return identity;
}

// the vector indicating the direction the model faces by default (with no rotation)
const glm::vec3& Entity::getDefaultFaceVector() const
{
	static glm::vec3 default_face_vec(1.0f, 0.0f, 0.0f);

	return default_face_vec;
}

void Entity::detachChild(Entity* const& child)
{
	for (size_t i = 0, len = this->children.size(); i < len; i++) {
		if (this->children[i] == child) {
			this->children.erase(this->children.begin() + i);
			return;
		}
	}
}

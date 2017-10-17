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

#include "entity.hpp"
#include "utils.hpp"

Entity::Entity(Entity* parent)
{
	// The parent entity's model matrix will be multiplied against this
	// entity's own transformation matrix when the model matrix is requested.
	// If no parent is passed, then we'll ignore that field.
	this->parent = parent;

	// can be toggled with this->hide(), this->unhide().
	this->hidden = false;

	// the default draw mode will be overridden by derived classes
	this->draw_mode = GL_LINES;
}

GLenum Entity::getDrawMode()
{
	return this->draw_mode;
}

const glm::mat4& Entity::getModelMatrix()
{
	static glm::mat4 identity;

	// if we have a parent entity we want to adjust our transformation
	// to incorporate the context of the parent's transformation
	glm::mat4 parent_model_matrix = this->parent ? this->parent->getModelMatrix() : identity;

	this->model_matrix =
			parent_model_matrix *
			this->translation_matrix * this->getBaseTranslation() *
			this->rotation_matrix * this->getBaseRotation() *
			this->scale_matrix * this->getBaseScale();
	return this->model_matrix;
}

glm::vec3 Entity::getPosition()
{
	return utils::getTranslationVector(this->translation_matrix);
}

bool Entity::isHidden()
{
	return this->hidden;
}

void Entity::scale(const float& scalar)
{
	this->scale_matrix = glm::scale(this->scale_matrix, glm::vec3(scalar));
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

void Entity::moveUp(const int& units)
{
	static glm::vec3 up_vec = glm::vec3(0.0f, 1.0f, 0.0f);

	this->translation_matrix = glm::translate(this->translation_matrix, (float)units * up_vec);
	this->orient((float)(M_PI / 2));
}

void Entity::moveDown(const int& units)
{
	static glm::vec3 down_vec = glm::vec3(0.0f, -1.0f, 0.0f);

	this->translation_matrix = glm::translate(this->translation_matrix, (float)units * down_vec);
	this->orient((float)(3 * M_PI / 2));
}

void Entity::moveLeft(const int& units)
{
	static glm::vec3 left_vec = glm::vec3(-1.0f, 0.0f, 0.0f);

	this->translation_matrix = glm::translate(this->translation_matrix, (float)units * left_vec);
	this->orient((float)M_PI);
}

void Entity::moveRight(const int& units)
{
	static glm::vec3 right_vec = glm::vec3(1.0f, 0.0f, 0.0f);

	this->translation_matrix = glm::translate(this->translation_matrix, (float)units * right_vec);
	this->orient(0.0f);
}

void Entity::setPosition(const float& x, const float& y, const float& z)
{
	static glm::mat4 identity;

	this->translation_matrix = glm::translate(identity, glm::vec3(x, y, z));
}

void Entity::setDrawMode(const GLenum& draw_mode)
{
	this->draw_mode = draw_mode;
}

void Entity::hide()
{
	this->hidden = true;
}

void Entity::unhide()
{
	this->hidden = false;
}

void Entity::orient(const float& angle)
{
	static glm::mat4 identity;
	static glm::vec3 z_axis = glm::vec3(0.0f, 0.0f, 1.0f);

	// re-write our rotation matrix to orient our model at the given
	// angle in respect to the z axis.
	this->rotation_matrix = glm::rotate(identity, angle, z_axis);
}

GLuint Entity::initVertexArray(
	const GLuint &shader_program,
	const std::vector<glm::vec3> &vertices,
	GLuint* vertices_buffer,
	GLuint* element_buffer
) {
	// if no elements vector is provided, we'll create a default

	std::vector<GLuint> elements;
	// naive default approach - traverse vertices in original order
	for (int i = 0, limit = (int)vertices.size(); i < limit; i++) {
		elements.emplace_back(i);
	}

	return Entity::initVertexArray(shader_program, vertices, elements, vertices_buffer, element_buffer);
}

GLuint Entity::initVertexArray(
	const GLuint& shader_program,
	const std::vector<glm::vec3>& vertices,
	const std::vector<GLuint>& elements,
	GLuint* vertices_buffer,
	GLuint* element_buffer
) {
	// Set VAO (Vertex Array Object) id
	GLuint vao;
	glGenVertexArrays(1, &vao);

	// Bind the VAO
	glBindVertexArray(vao);

	// Create vertices buffer
	GLuint v_buff_temp;
	if (!vertices_buffer) {
		vertices_buffer = &v_buff_temp;
	}
	glGenBuffers(1, vertices_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vertices_buffer);
	glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glm::vec3),
			&vertices.front(),
			GL_STATIC_DRAW
	);

	// Bind attribute pointer for the vertices buffer
	auto v_position = (GLuint)glGetAttribLocation(shader_program, "v_position");
	glEnableVertexAttribArray(v_position);
	glVertexAttribPointer(v_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

	// Create element buffer
	GLuint e_buff_temp;
	if (!element_buffer) {
		element_buffer = &e_buff_temp;
	}
	glGenBuffers(1, element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *element_buffer);
	glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			elements.size() * sizeof(GLuint),
			&elements.front(),
			GL_STATIC_DRAW
	);

	// Unbind VAO, then the corresponding buffers.
	// VAO should be unbound BEFORE element array buffer so VAO remembers
	// the last bound element array buffer!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return vao;
}

// derived classes should override this if the model
// needs a fundamental scale offset
const glm::mat4& Entity::getBaseScale()
{
	static glm::mat4 identity;
	return identity;
}

// derived classes should override this if the model
// needs a fundamental rotation offset
const glm::mat4& Entity::getBaseRotation()
{
	static glm::mat4 identity;
	return identity;
}

// derived classes should override this if the model
// needs a fundamental translation offset
const glm::mat4& Entity::getBaseTranslation()
{
	static glm::mat4 identity;
	return identity;
}

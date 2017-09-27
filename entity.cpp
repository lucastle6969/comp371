#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "entity.hpp"

Entity::Entity(Entity* parent)
{
	// The parent entity's model matrix will be multiplied against this
	// entity's own transformation matrix when the model matrix is requested.
	// If no parent is passed, then we'll ignore that field.
	this->parent = parent;

	// this->vertices may optionally be defined by derived class constructor
	this->vertices = nullptr;

	// this->vao will be defined when the initVertexArray() method is called,
	// but only if this->vertices has already been defined.
	// DERIVING CLASSES SHOULD CALL initVertexArray()!
	this->vao = nullptr;
}

Entity::~Entity()
{
	// since parent was initialized externally, we don't need
	// to deal with its de-allocation.

	delete this->vertices;
	delete this->vao;
}

std::vector<glm::vec3>* Entity::getVertices()
{
	return this->vertices;
}

GLuint* Entity::getVAO()
{
	return this->vao;
}

const int* Entity::getColorType()
{
	return nullptr;
}

const GLenum* Entity::getDrawMode()
{
	return nullptr;
}

glm::mat4& Entity::getModelMatrix()
{
	static glm::mat4 identity;

	// if we have a parent entity we want to adjust our transformation
	// to incorporate the context of the parent's transformation
	glm::mat4 parent_model_matrix = this->parent ? this->parent->getModelMatrix() : identity;

	this->model_matrix =
			parent_model_matrix *
			this->translation_matrix *
			this->rotation_matrix *
			this->scale_matrix;
	return this->model_matrix;
}

void Entity::scale(const float& scalar)
{
	this->scale_matrix = glm::scale(this->scale_matrix, glm::vec3(scalar));
}

void Entity::moveUp(const int& units = 1)
{
	static glm::vec3 up_vec = glm::vec3(0.0f, 1.0f, 0.0f);

	this->translation_matrix = glm::translate(this->translation_matrix, (float)units * up_vec);
	this->orient((float)(M_PI / 2));
}

void Entity::moveDown(const int& units = 1)
{
	static glm::vec3 down_vec = glm::vec3(0.0f, -1.0f, 0.0f);

	this->translation_matrix = glm::translate(this->translation_matrix, (float)units * down_vec);
	this->orient((float)(3 * M_PI / 2));
}

void Entity::moveLeft(const int& units = 1)
{
	static glm::vec3 left_vec = glm::vec3(-1.0f, 0.0f, 0.0f);

	this->translation_matrix = glm::translate(this->translation_matrix, (float)units * left_vec);
	this->orient((float)(2 * M_PI));
}

void Entity::moveRight(const int& units = 1)
{
	static glm::vec3 right_vec = glm::vec3(1.0f, 0.0f, 0.0f);

	this->translation_matrix = glm::translate(this->translation_matrix, (float)units * right_vec);
	this->orient(0.0f);
}

void Entity::orient(const float& angle)
{
	static glm::mat4 identity;
	static glm::vec3 z_axis = glm::vec3(0.0f, 0.0f, 1.0f);

	// re-write our rotation matrix to orient our model at the given
	// angle in respect to the z axis.
	this->rotation_matrix = glm::rotate(identity, angle, z_axis);
}

void Entity::initVertexArray(const GLuint& shader_program)
{
	// this->vertices is never defined by the Entity class,
	// but it may be defined by a derived class.
	if (this->vertices == nullptr || this->vao != nullptr) {
		return;
	}

	// set VAO (Vertex Array Object) id
	this->vao = new GLuint;
	glGenVertexArrays(1, this->vao);

	// Bind the VAO first
	glBindVertexArray(*this->vao);

	// create vertices buffer
	GLuint vertices_buffer;
	glGenBuffers(1, &vertices_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
	glBufferData(
			GL_ARRAY_BUFFER,
			this->vertices->size() * sizeof(glm::vec3),
			&this->vertices->front(),
			GL_STATIC_DRAW
	);

	// bind attribute pointers
	auto v_position = (GLuint)glGetAttribLocation(shader_program, "v_position");
	glEnableVertexAttribArray(v_position);
	glVertexAttribPointer(v_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

	// unbind buffer and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

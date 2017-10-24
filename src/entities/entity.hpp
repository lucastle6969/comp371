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
	void orient(const glm::vec3& forward_vec);

protected:
	GLenum draw_mode;
	static GLuint initVertexArray(
			const GLuint& shader_program,
			const std::vector<glm::vec3>& vertices,
			GLuint* vertices_buffer = nullptr,
			GLuint* element_buffer = nullptr
	);
	static GLuint initVertexArray(
		const GLuint& shader_program,
		const std::vector<glm::vec3>& vertices,
		const std::vector<GLuint>& elements,
		GLuint* vertices_buffer = nullptr,
		GLuint* element_buffer = nullptr
	);
	virtual const glm::mat4& getBaseScale();
	virtual const glm::mat4& getBaseRotation();
	virtual const glm::mat4& getBaseTranslation();
	virtual const glm::vec3& getDefaultFaceVector();

public:
	Entity() : Entity(nullptr) {}
	virtual ~Entity() = default;
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
	void moveForward(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f);
	void moveBack(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f);
	void moveLeft(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f);
	void moveRight(const glm::vec3& view_vec, const glm::vec3& up_vec, const float& units = 1.0f);
	void setPosition(const glm::vec3& position);
	void setDrawMode(const GLenum& draw_mode);
	void hide();
	void unhide();
	void toggle_hide();
};

#endif //PACMAN3D_ENTITY_H
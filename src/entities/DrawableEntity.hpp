#ifndef PROCEDURALWORLD_DRAWABLEENTITY_HPP
#define PROCEDURALWORLD_DRAWABLEENTITY_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>
#include "Light.h"
#include "Entity.hpp"

// Abstract class

class DrawableEntity : public Entity {
private:
	GLuint shader_program;

protected:
	GLenum draw_mode;

	  //\\  |\    /||\    /|
	 //  \\ ||\  /||||\  /||
	//----\\||\\//||||\\//||

	glm::vec3 color;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	GLuint initVertexArray(
			const std::vector<glm::vec3>& vertices,
			const std::vector<GLuint>& elements,
			const std::vector<glm::vec2>& uvs,
			const std::vector<glm::vec3>& normals,
			glm::vec3 surfaceColor,
			glm::vec3 lightDiffuse,
			glm::vec3 lightSpecular,
			float lightShininess
	);

	  //\\  |\    /||\    /|
	 //  \\ ||\  /||||\  /||
	//----\\||\\//||||\\//||

	GLuint initVertexArray(const std::vector<glm::vec3>& vertices, GLuint* vertices_buffer = nullptr, GLuint* element_buffer = nullptr);
	GLuint initVertexArray(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& elements, GLuint* vertices_buffer = nullptr, GLuint* element_buffer = nullptr);

public:
	explicit DrawableEntity(const GLuint& shader_program) : DrawableEntity(shader_program, nullptr) {}
	DrawableEntity(const GLuint& shader_program, Entity* parent);
	// pure virtual methods must be overridden by derived classes
	virtual const std::vector<glm::vec3>& getVertices() = 0;
	virtual GLuint getVAO() = 0;
	virtual const int getColorType() = 0;
	// end pure virtual functions
	GLenum getDrawMode();
	void setDrawMode(const GLenum& draw_mode);
	void draw(const glm::mat4& view_matrix, const glm::mat4& projection_matrix, Light light) override;
};

#endif // PROCEDURALWORLD_DRAWABLEENTITY_HPP

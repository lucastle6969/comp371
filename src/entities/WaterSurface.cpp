#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <vector>

#include <src/constants.hpp>

#include "DrawableEntity.hpp"
#include "WaterSurface.hpp"

WaterSurface::WaterSurface(const GLuint &shader_program, Entity* parent)
	: DrawableEntity(shader_program, parent)
{
	this->draw_mode = GL_TRIANGLES;
	this->setMaterial(
		glm::vec3(0.023f, 0.07f, 0.082f),
		glm::vec3(0.23f, 0.7f, 0.82f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		10.0f
	);
}

const std::vector<glm::vec3>& WaterSurface::getVertices() const
{
	static const std::vector<glm::vec3> vertices = {
			glm::vec3(0.0f, 0.0f, 0.0f), // bottom-left
			glm::vec3(1.0f, 0.0f, 0.0f), // bottom-right
			glm::vec3(1.0f, 0.0f, 1.0f), // top-right
			glm::vec3(0.0f, 0.0f, 1.0f)  // top-left
	};

	return vertices;
}

GLuint WaterSurface::getVAO() {
	static const std::vector<GLuint> elements {
			// first triangle (ACTUALLY is counterclockwise - negative-Z axis)
			3, // top-left
			1, // bottom-right
			0, // bottom-left
			// second triangle
			3, // top-left
			2, // top-right
			1  // bottom-right
	};

	static const std::vector<glm::vec3> normals {
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
	};

	static GLuint vao;
	static bool vao_init = false;

	if (!vao_init) {
		// only initialize vao once for all instances
		vao = this->initVertexArray(
				this->getVertices(),
				elements,
				normals
		);
		vao_init = true;
	}

	return vao;
}

const int WaterSurface::getColorType() {
	return COLOR_LIGHTING;
}

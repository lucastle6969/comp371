#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "Entity.hpp"
#include "DrawableEntity.hpp"

#include "Skybox.hpp"
#include "../utils.hpp"
#include "../constants.hpp"
#include "../loadTexture.hpp"
#include "../objloader.hpp"


float size = 10;

Skybox::Skybox(const GLuint& shader_program, Entity* parent) : DrawableEntity(shader_program, parent)
{
    this->draw_mode = GL_TRIANGLES;

    this->setMaterial(
            glm::vec3(.5, .5, .5), // need to change this to some other value... maybe the height of the plane if we ever make it.
            glm::vec3(.5, .5, .5),
            glm::vec3(.25, .25, .25),
            25.0f
    );

	std::vector<GLuint> elements;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	loadOBJ("../models/domeshpere.obj", &this->verticies, &normals, &uvs, &elements);

	this->vao = this->initVertexArray(
			this->getVertices(),
			elements,
			normals,
			uvs
	);
}

const std::vector<glm::vec3>& Skybox::getVertices() const {
	return this->verticies;
}

GLuint Skybox::getVAO()
{
	return this->vao;
}

const int Skybox::getColorType() {
    return COLOR_SKY_TEXTURE;
}

GLuint Skybox::getTextureId()
{
    //from pinterest
    static GLuint cloud_texture = loadTexture(
            "../textures/nightskydome1.jpg",
            GL_LINEAR,
            GL_LINEAR
    );
    return cloud_texture;
}

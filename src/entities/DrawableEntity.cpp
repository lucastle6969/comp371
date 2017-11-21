#ifdef __APPLE__

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <stdexcept>

#include "Light.h"
#include "Entity.hpp"
#include "DrawableEntity.hpp"

DrawableEntity::DrawableEntity(const GLuint &shader_program, Entity *parent) : Entity(parent) {
    static std::string invalid_program_error = "Must supply valid shader program id.";
    static std::string invalid_operation_error = "There was an error using the shader program.";

    if (shader_program == 0) {
        throw std::runtime_error(invalid_program_error);
    }

    // clear any outstanding GL errors
    while (glGetError() != GL_NO_ERROR) {/* do nothing */}

    glUseProgram(shader_program);

    // test for shader program errors
    GLenum gl_error;
    while ((gl_error = glGetError()) != GL_NO_ERROR) {
        if (gl_error == GL_INVALID_VALUE) {
            throw std::runtime_error(invalid_program_error);
        }
        if (gl_error == GL_INVALID_OPERATION) {
            throw std::runtime_error(invalid_program_error);
        }
    }

    glUseProgram(0);

    this->shader_program = shader_program;

    // the default draw mode will be overridden by derived classes
    this->draw_mode = GL_LINES;
}

GLenum DrawableEntity::getDrawMode() {
    return this->draw_mode;
}

GLuint DrawableEntity::getTextureId()
{
    static GLuint texture_id;
    return texture_id;
}

void DrawableEntity::draw(
    const glm::mat4& view_matrix,
    const glm::mat4& projection_matrix,
    const Light& light
) {
	Entity::draw(view_matrix, projection_matrix, light);

	if (this->isHidden()) {
		return;
	}

	auto mvp_matrix_loc = (GLuint)glGetUniformLocation(this->shader_program, "mvp_matrix");
    auto model_loc = (GLuint)glGetUniformLocation(this->shader_program, "model");
    auto color_type_loc = (GLuint)glGetUniformLocation(this->shader_program, "color_type");
	auto position_x_loc = (GLuint)glGetUniformLocation(this->shader_program, "entity_position_x");
	auto position_z_loc = (GLuint)glGetUniformLocation(this->shader_program, "entity_position_z");
    auto sunVector_loc =(GLuint)glGetUniformLocation(this->shader_program, "sunVector");
    auto lightAmbient_loc = (GLuint)glGetUniformLocation(this->shader_program, "lightAmbient");
    auto lightDiffuse_loc = (GLuint)glGetUniformLocation(this->shader_program, "lightDiffuse");
    auto lightSpecular_loc = (GLuint)glGetUniformLocation(this->shader_program, "lightSpecular");
    auto shininess_loc = (GLuint)glGetUniformLocation(this->shader_program, "shininess");
    auto worldViewPos_loc = (GLuint)glGetUniformLocation(this->shader_program, "worldViewPos");
    //auto sunPosition_loc = (GLuint)glGetUniformLocation(this->shader_program, "sunPosition");

    glUseProgram(this->shader_program);

	const glm::mat4& model_matrix = this->getModelMatrix();
	// use the entity's model matrix to form a new Model View Projection matrix
	glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;
	glUniformMatrix4fv(mvp_matrix_loc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniform1i(color_type_loc, this->getColorType());
	// send the position of this entity to the shader
	glm::vec3 position = this->getPosition();
	glUniform1i(position_x_loc, (GLint)position.x);
	glUniform1i(position_z_loc, (GLint)position.z);
	glUniform3fv(sunVector_loc, 1, glm::value_ptr(light.light_direction));
	glUniform3fv(lightAmbient_loc, 1, glm::value_ptr(this->ambient));
	glUniform3fv(lightDiffuse_loc, 1, glm::value_ptr(this->diffuse));
	glUniform3fv(lightSpecular_loc, 1, glm::value_ptr(this->specular));
	glUniform1f(shininess_loc, this->shininess);
	// compute world view position from inverse view matrix
	// thanks: https://www.opengl.org/discussion_boards/showthread.php/178484-Extracting-camera-position-from-a-ModelView-Matrix
	glm::vec3 world_view_position(glm::inverse(view_matrix)[3]);
	glUniform3fv(worldViewPos_loc, 1, glm::value_ptr(world_view_position));

	// TODO: figure out why the commented-out code below fails on macOS
	// glUniform1i(tex_image_loc, GL_TEXTURE0);
	// glActiveTexture(GL_TEXTURE0);

	// Draw
	glBindVertexArray(this->getVAO());
	GLenum draw_mode = this->getDrawMode();
	if (draw_mode == GL_POINTS) {
		// it's inefficient and useless to use glDrawElements for a point cloud
		glDrawArrays(draw_mode, 0, (GLuint) this->getVertices().size());
	} else {
		glBindTexture(GL_TEXTURE_2D, this->getTextureId());

		int element_buffer_array_size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &element_buffer_array_size);
		glDrawElements(
			draw_mode,
			element_buffer_array_size / sizeof(GLuint),
			GL_UNSIGNED_INT,
			nullptr
		);
	}
	glBindVertexArray(0);

	glUseProgram(0);
}

GLuint DrawableEntity::initVertexArray(
        const std::vector<glm::vec3>& vertices,
        GLuint* vertices_buffer,
        GLuint* element_buffer
) {
    // if no elements vector is provided, we'll create a default

    std::vector<GLuint> elements;
    // naive default approach - traverse vertices in original order
    for (int i = 0, limit = (int) vertices.size(); i < limit; i++) {
        elements.emplace_back(i);
    }

    return this->initVertexArray(vertices, elements, vertices_buffer, element_buffer);
}

GLuint DrawableEntity::initVertexArray(
        const std::vector<glm::vec3>& vertices,
        const std::vector<GLuint>& elements,
        const std::vector<glm::vec3>& normals,
        const std::vector<glm::vec2>& uvs,
        GLuint* vertices_buffer,
        GLuint* element_buffer,
        GLuint* normal_buffer,
        GLuint* uv_buffer
) {
    GLuint vao = this->initVertexArray(
	    vertices,
	    elements,
	    normals,
	    vertices_buffer,
	    element_buffer,
	    normal_buffer
    );

    // Bind the VAO
    glBindVertexArray(vao);

    // Use the shader program
    glUseProgram(this->shader_program);

    // Create texture coordinates buffer
    GLuint uv_buff_temp;
    if (!uv_buffer) {
        uv_buffer = &uv_buff_temp;
    }
    glGenBuffers(1, uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, *uv_buffer);
    glBufferData(
            GL_ARRAY_BUFFER,
            uvs.size() * sizeof(glm::vec2),
            &uvs.front(),
            GL_STATIC_DRAW
    );

    // Bind attribute pointer for the texture coordinates buffer
    auto tex_coord_in = (GLuint) glGetAttribLocation(this->shader_program, "tex_coord_in");
    glEnableVertexAttribArray(tex_coord_in);
    glVertexAttribPointer(tex_coord_in, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    // Unbind VAO, then the corresponding buffers.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unset shader program
    glUseProgram(0);

    return vao;
}

GLuint DrawableEntity::initVertexArray(
	const std::vector<glm::vec3>& vertices,
	const std::vector<GLuint>& elements,
	const std::vector<glm::vec3>& normals,
	GLuint* vertices_buffer,
	GLuint* element_buffer,
	GLuint* normal_buffer
){
	GLuint vao = this->initVertexArray(vertices, elements, vertices_buffer, element_buffer);

	// Bind the VAO
	glBindVertexArray(vao);

	// create normal buffer
	GLuint n_buff_temp;
	if (!normal_buffer) {
		normal_buffer = &n_buff_temp;
	}
	glGenBuffers(1, normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *normal_buffer);
	glBufferData(
		GL_ARRAY_BUFFER,
		normals.size() * sizeof(glm::vec3),
		&normals.front(),
		GL_STATIC_DRAW
	);

	// Bind attribute pointer for the normal buffer
	auto normal = (GLuint)glGetAttribLocation(this->shader_program, "normal");
	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

	// Unbind VAO, then the corresponding buffers.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unset shader program
	glUseProgram(0);

	return vao;
}

GLuint DrawableEntity::initVertexArray(
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

	// Use the shader program
	glUseProgram(this->shader_program);

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
	auto v_position = (GLuint) glGetAttribLocation(this->shader_program, "v_position");
	glEnableVertexAttribArray(v_position);
	glVertexAttribPointer(v_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
	                      nullptr);

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

	// Unset shader program
	glUseProgram(0);

	return vao;
}

void DrawableEntity::setMaterial(
	const glm::vec3& lightAmbient,
	const glm::vec3& lightDiffuse,
	const glm::vec3& lightSpecular,
	const float& lightShininess
) {
	this->ambient = lightAmbient;
	this->diffuse = lightDiffuse;
	this->specular = lightSpecular;
	this->shininess = lightShininess;
}

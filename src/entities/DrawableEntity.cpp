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
#include <climits>

#include <src/ShaderUniformLocationCache.hpp>
#include <src/constants.hpp>

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

	// don't draw back faces by default
	this->should_draw_back_face = false;
}

GLenum DrawableEntity::getDrawMode() {
    return this->draw_mode;
}

GLuint DrawableEntity::getTextureId()
{
    static GLuint texture_id = UINT_MAX; // default value
    return texture_id;
}

// NOTE: this method iterates all vertices each time it is called
float DrawableEntity::getWorldHeight() const
{
	float min = FLT_MAX;
	float max = -FLT_MAX;
	glm::vec3 v;
	const glm::mat4& model_matrix = this->getModelMatrix();
	for (const glm::vec3& vertex : this->getVertices()) {
		v = glm::vec3(model_matrix * glm::vec4(vertex, 1.0f));
		if (v.y < min) {
			min = v.y;
		}
		if (v.y > max) {
			max = v.y;
		}
	}
	return max - min;
}

void DrawableEntity::drawSelf(
    const glm::mat4& view_matrix,
    const glm::mat4& projection_matrix,
    const Light& light
) {
	if (this->isHidden()) {
		return;
	}

	GLuint mvp_matrix_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::mvp_matrix
	);
	GLuint model_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::model
	);
	GLuint color_type_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::color_type
	);
	GLuint position_x_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::position_x
	);
	GLuint position_z_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::position_z
	);
	GLuint opacity_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::opacity
	);
	GLuint worldViewPos_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::worldViewPos
	);

	GLuint material_ambient_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::material_ambient
	);
	GLuint material_diffuse_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::material_diffuse
	);
	GLuint material_specular_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::material_specular
	);
	GLuint material_shininess_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::material_shininess
	);

	GLuint sun_direction_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::sun_direction
	);
	GLuint sun_color_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::sun_color
	);
	GLuint point_light_pos_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::point_light_pos
	);
	GLuint point_light_color_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::point_light_color
	);

	GLuint use_texture_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::use_texture
	);

	GLuint fog_color_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::fog_color
	);
	GLuint daytime_value_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::daytime_value
	);
	GLuint nighttime_value_loc = ShaderUniformLocationCache::getLocation(
		this->shader_program,
		SHADER_UNIFORMS::nighttime_value
	);

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
	glUniform1f(opacity_loc, this->getOpacity());
	// compute world view position from inverse view matrix
	// thanks: https://www.opengl.org/discussion_boards/showthread.php/178484-Extracting-camera-position-from-a-ModelView-Matrix
	glm::vec3 world_view_position(glm::inverse(view_matrix)[3]);
	glUniform3fv(worldViewPos_loc, 1, glm::value_ptr(world_view_position));


    glUniform3fv(sun_direction_loc, 1, glm::value_ptr(light.light_direction));
	glUniform3fv(sun_color_loc, 1, glm::value_ptr(light.color));
	// TODO: make the point light follow the player? or remove it?
	glUniform3fv(point_light_pos_loc, 1, glm::value_ptr(light.light_position));
	// TODO: define the point light color somewhere else or remove it
	glUniform3fv(point_light_color_loc, 1, glm::value_ptr(light.position_light_color));
    glUniform3fv(material_ambient_loc, 1, glm::value_ptr(this->ambient));
    glUniform3fv(material_diffuse_loc, 1, glm::value_ptr(this->diffuse));
    glUniform3fv(material_specular_loc, 1, glm::value_ptr(this->specular));
    glUniform1f(material_shininess_loc, this->shininess);

	glUniform3fv(fog_color_loc, 1, glm::value_ptr(light.fog_color));
	glUniform1f(daytime_value_loc, light.daytime_value);
	glUniform1f(nighttime_value_loc, light.nighttime_value);

	GLuint texture_id = this->getTextureId();
	glUniform1i(use_texture_loc, texture_id != UINT_MAX);

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
		glBindTexture(GL_TEXTURE_2D, texture_id);

		int element_buffer_array_size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE,
		                       &element_buffer_array_size);

		if (this->should_draw_back_face) {
			glDisable(GL_CULL_FACE);
		}

		glDrawElements(
				draw_mode,
				element_buffer_array_size / sizeof(GLuint),
				GL_UNSIGNED_INT,
				nullptr
		);

		// we don't want to assume everything will be drawn with the DrawableEntity
		// class so we should restore the default setting after drawing
		glEnable(GL_CULL_FACE);
	}

	glBindVertexArray(0);

	glUseProgram(0);
}

void DrawableEntity::drawIfOpaque(
	const glm::mat4& view_matrix,
	const glm::mat4& projection_matrix,
	const Light& light
) {
	Entity::drawIfOpaque(view_matrix, projection_matrix, light);
	if (this->getOpacity() >= 1.0f) {
		this->drawSelf(view_matrix, projection_matrix, light);
	}
}

void DrawableEntity::drawIfTransparent(
	const glm::mat4& view_matrix,
	const glm::mat4& projection_matrix,
	const Light& light
) {
	Entity::drawIfTransparent(view_matrix, projection_matrix, light);
	if (this->getOpacity() < 1.0f) {
		this->drawSelf(view_matrix, projection_matrix, light);
	}
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
	const glm::vec3& ambient,
	const glm::vec3& diffuse,
	const glm::vec3& specular,
	const float& shininess
) {
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
}

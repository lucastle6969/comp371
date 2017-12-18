#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <string>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include <src/vendor/stb_image.h>

#include "constants.hpp"
#include "loadTexture.hpp"

GLuint loadTexture(const std::string& path)
{
	return loadTexture(
			path,
			DEFAULT_MINIFICATION_FILTER,
			DEFAULT_MAGNIFICATION_FILTER,
			DEFAULT_TEXTURE_WRAP,
			DEFAULT_TEXTURE_WRAP,
			DEFAULT_STB_VERT_ALIGN_FLIP
	);
}

GLuint loadTexture(const std::string& path, const bool& vert_align_flip)
{
	return loadTexture(
			path,
			DEFAULT_MINIFICATION_FILTER,
			DEFAULT_MAGNIFICATION_FILTER,
			DEFAULT_TEXTURE_WRAP,
			DEFAULT_TEXTURE_WRAP,
			vert_align_flip
	);
}

GLuint loadTexture(
	const std::string& path,
	const GLint& min_filter,
	const GLint& mag_filter
) {
	return loadTexture(
			path,
			min_filter,
			mag_filter,
			DEFAULT_TEXTURE_WRAP,
			DEFAULT_TEXTURE_WRAP,
			DEFAULT_STB_VERT_ALIGN_FLIP
	);
}

GLuint loadTexture(
	const std::string& path,
	const GLint& min_filter,
	const GLint& mag_filter,
	const bool& vert_align_flip
) {
	return loadTexture(
			path,
			min_filter,
			mag_filter,
			DEFAULT_TEXTURE_WRAP,
			DEFAULT_TEXTURE_WRAP,
			vert_align_flip
	);
}

GLuint loadTexture(
	const std::string& path,
	const GLint& min_filter,
	const GLint& mag_filter,
	const GLint& texture_wrap_s,
	const GLint& texture_wrap_t
) {
	return loadTexture(
			path,
			min_filter,
			mag_filter,
			texture_wrap_s,
			texture_wrap_t,
			DEFAULT_STB_VERT_ALIGN_FLIP
	);
}

GLuint loadTexture(
	const std::string& path,
	const GLint& min_filter,
	const GLint& mag_filter,
	const GLint& texture_wrap_s,
	const GLint& texture_wrap_t,
	const bool& vert_align_flip
) {
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	//set wrapping params
	// GL_MIRRORED_REPEAT is default?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_wrap_t);

	//set texture filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	//load image, create texture and generate mipmaps
	int texture_width, texture_height, texture_channels;

	stbi_set_flip_vertically_on_load(vert_align_flip);
	unsigned char* texture_data = stbi_load(
			path.c_str(),
			&texture_width,
			&texture_height,
			&texture_channels,
			4
	);
	stbi_set_flip_vertically_on_load(false);
	if (texture_data) {
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				texture_width,
				texture_height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				texture_data
		);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		throw std::runtime_error("Failed to load image \'" + path + "\': ");
	}
	stbi_image_free(texture_data);

	return texture_id;
}

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

GLuint loadTexture(
	const std::string& path,
	const GLint& min_filter,
	const GLint& mag_filter
) {
	GLuint rock_texture;
	glGenTextures(1, &rock_texture);
	glBindTexture(GL_TEXTURE_2D, rock_texture);

	//set wrapping params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	//set texture filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	//load image, create texture and generate mipmaps
	int rock_tex_width, rock_tex_height, rock_tex_nrChannels;

	unsigned char* rock_tex_data = stbi_load(
			path.c_str(),
			&rock_tex_width,
			&rock_tex_height,
			&rock_tex_nrChannels,
			0
	);
	if (rock_tex_data) {
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				rock_tex_width,
				rock_tex_height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				rock_tex_data
		);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		throw std::runtime_error("Failed to load image \'" + path + "\': ");
	}
	stbi_image_free(rock_tex_data);

	return rock_texture;
}
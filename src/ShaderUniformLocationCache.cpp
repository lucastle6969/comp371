#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <map>

#include "ShaderUniformLocationCache.hpp"

std::map<GLuint, std::map<int, GLuint>> ShaderUniformLocationCache::uniforms_locations;

GLuint ShaderUniformLocationCache::getLocation(
	const GLuint& shader_program,
	const int& uniform_var_id
) {
	return ShaderUniformLocationCache::uniforms_locations
		.at(shader_program)
		.at(uniform_var_id);
}

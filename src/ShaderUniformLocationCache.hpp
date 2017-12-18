#ifndef PROCEDURALWORLD_SHADERUNIFORMLOCATIONCACHE_HPP
#define PROCEDURALWORLD_SHADERUNIFORMLOCATIONCACHE_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <map>

// by caching the locations corresponding to relevant uniform variables in each
// shader program, we can avoid the signficant overhead cost of querying these
// variable locations each time they are needed.

class ShaderUniformLocationCache {
	friend GLuint prepareShaderProgram(
		const std::string& vertex_shader_path,
		const std::string& fragment_shader_path,
		bool* const& ok
	);
	static std::map<GLuint, std::map<int, GLuint>> uniforms_locations;
public:
	static GLuint getLocation(
		const GLuint& shader_program,
		const int& uniform_var_id
	);
};


#endif //PROCEDURALWORLD_SHADERUNIFORMLOCATIONCACHE_HPP

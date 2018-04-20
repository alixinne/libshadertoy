#include <algorithm>
#include <vector>

#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/caller.hpp"
#include "shadertoy/gl/shader.hpp"

using namespace shadertoy::gl;

null_shader_error::null_shader_error()
	: shadertoy_error("An attempt was made to dereference a null shader")
{
}

shader_compilation_error::shader_compilation_error(GLuint shaderId, const std::string &log)
	: shadertoy_error("OpenGL shader compilation error"),
	shader_id_(shaderId),
	log_(log)
{
}

GLuint shader_allocator::create(GLenum shaderType)
{
	return gl_call(glCreateShader, shaderType);
}

void shader_allocator::destroy(GLuint resource)
{
	gl_call(glDeleteShader, resource);
}

shader::shader(GLenum shaderType)
	: resource(allocator_type().create(shaderType))
{
}

void shader::source(const std::string &string) const
{
	const GLchar *cstr = string.c_str();

	gl_call(glShaderSource, GLuint(*this), 1, &cstr, nullptr);
}

void shader::source(const std::vector<std::string> &string) const
{
	std::vector<const char *> cstr(string.size());

	std::transform(string.begin(), string.end(), cstr.begin(),
		[] (const std::string &src) {
			return src.c_str();
		});

	gl_call(glShaderSource, GLuint(*this), string.size(), cstr.data(), nullptr);
}

void shader::compile() const
{
	// Try to compile shader
	gl_call(glCompileShader, GLuint(*this));

	// Get status
	GLint compileStatus;
	gl_call(glGetShaderiv, GLuint(*this), GL_COMPILE_STATUS, &compileStatus);

	// Throw error if shader could not be compiled
	if (compileStatus != GL_TRUE)
	{
		throw shader_compilation_error(GLuint(*this), this->log());
	}
}

std::string shader::log() const
{
	// Get log length
	GLint infoLogLength = 0;
	gl_call(glGetShaderiv, GLuint(*this), GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength == 0)
		return std::string();

	// Get log
	std::vector<GLchar> logStr(infoLogLength);
	gl_call(glGetShaderInfoLog, GLuint(*this), infoLogLength, nullptr, logStr.data());

	// exclude the null character from the range passed to string constructor
	return std::string(logStr.begin(), logStr.end() - 1);
}

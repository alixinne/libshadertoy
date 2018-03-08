#include <algorithm>
#include <vector>

#include <epoxy/gl.h>

#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Caller.hpp"
#include "shadertoy/OpenGL/Shader.hpp"

using namespace shadertoy::OpenGL;

NullShaderError::NullShaderError()
	: ShadertoyError("An attempt was made to dereference a null shader")
{
}

ShaderCompilationError::ShaderCompilationError(GLuint shaderId, const std::string &log)
	: ShadertoyError("OpenGL shader compilation error"),
	_shaderId(shaderId),
	_log(log)
{
}

GLuint ShaderAllocator::Create(GLenum shaderType)
{
	return glCall(glCreateShader, shaderType);
}

void ShaderAllocator::Delete(GLuint resource)
{
	glCall(glDeleteShader, resource);
}

Shader::Shader(GLenum shaderType)
	: Resource(Allocator().Create(shaderType))
{
}

void Shader::Source(const std::string &string)
{
	const GLchar *cstr = string.c_str();

	glCall(glShaderSource, GLuint(*this), 1, &cstr, nullptr);
}

void Shader::Source(const std::vector<std::string> &string)
{
	std::vector<const char *> cstr(string.size());

	std::transform(string.begin(), string.end(), cstr.begin(),
		[] (const std::string &src) {
			return src.c_str();
		});

	glCall(glShaderSource, GLuint(*this), string.size(), cstr.data(), nullptr);
}

void Shader::Compile()
{
	// Try to compile shader
	glCall(glCompileShader, GLuint(*this));

	// Get status
	GLint compileStatus;
	glCall(glGetShaderiv, GLuint(*this), GL_COMPILE_STATUS, &compileStatus);

	// Throw error if shader could not be compiled
	if (compileStatus != GL_TRUE)
	{
		throw ShaderCompilationError(GLuint(*this), this->Log());
	}
}

std::string Shader::Log()
{
	// Get log length
	GLint infoLogLength = 0;
	glCall(glGetShaderiv, GLuint(*this), GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength == 0)
		return std::string();

	// Get log
	std::vector<GLchar> logStr(infoLogLength);
	glCall(glGetShaderInfoLog, GLuint(*this), infoLogLength, nullptr, logStr.data());

	// exclude the null character from the range passed to string constructor
	return std::string(logStr.begin(), logStr.end() - 1);
}

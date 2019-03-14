#include <utility>
#include <vector>

#include <epoxy/gl.h>

#include "shadertoy/gl/program.hpp"
#include "shadertoy/gl/shader.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::gl;

null_program_error::null_program_error()
	: shadertoy_error("An attempt was made to dereference a null program")
{
}

attrib_location::attrib_location(GLint location)
	: location_(location)
{
}

void attrib_location::vertex_pointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) const
{
	gl_call(glVertexAttribPointer, location_, size, type, normalized, stride, pointer);
}

void attrib_location::enable_vertex_array() const
{
	gl_call(glEnableVertexAttribArray, location_);
}

uniform_location::uniform_location(const program &program, GLint location)
	: program_(GLuint(program)),
	location_(location)
{
}

bool uniform_location::is_active() const
{
	return location_ != -1;
}

program_link_error::program_link_error(GLuint programId, std::string log)
: shadertoy_error("OpenGL program linking error"), program_id_(programId), log_(std::move(log))
{
}

program_validate_error::program_validate_error(GLuint programId, std::string log)
: shadertoy_error("OpenGL program validation error"), program_id_(programId), log_(std::move(log))
{
}

void program::link() const
{
	gl_call(glLinkProgram, GLuint(*this));

	GLint linkStatus;
	gl_call(glGetProgramiv, GLuint(*this), GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		throw program_link_error(GLuint(*this), this->log());
	}
}

void program::use() const
{
	gl_call(glUseProgram, GLuint(*this));
}

void program::validate() const
{
	gl_call(glValidateProgram, GLuint(*this));

	GLint validateStatus;
	gl_call(glGetProgramiv, GLuint(*this), GL_LINK_STATUS, &validateStatus);
	if (validateStatus != GL_TRUE)
	{
		throw program_validate_error(GLuint(*this), this->log());
	}
}

uniform_location program::get_uniform_location(const GLchar *name) const
{
	GLint location = gl_call(glGetUniformLocation, GLuint(*this), name);
	return { *this, location };
}

attrib_location program::get_attrib_location(const GLchar *name) const
{
	GLint location = gl_call(glGetAttribLocation, GLuint(*this), name);
	return { location };
}

void program::attach_shader(const shader &shader) const
{
	gl_call(glAttachShader, GLuint(*this), GLuint(shader));
}

void program::detach_shader(const shader &shader) const
{
	gl_call(glDetachShader, GLuint(*this), GLuint(shader));
}

std::string program::log() const
{
	// Get log length
	GLint infoLogLength = 0;
	gl_call(glGetProgramiv, GLuint(*this), GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength == 0)
	{
		return std::string();
	}

	// Get log
	std::vector<GLchar> logStr(infoLogLength);
	gl_call(glGetProgramInfoLog, GLuint(*this), infoLogLength, nullptr, logStr.data());

	// exclude the null character from the range passed to string constructor
	return std::string(logStr.begin(), logStr.end() - 1);
}

void program::get(GLenum pname, GLint *params) const
{
	gl_call(glGetProgramiv, GLuint(*this), pname, params);
}

void program::get_binary(GLsizei bufsize, GLsizei *length, GLenum *binaryFormat, void *binary) const
{
	gl_call(glGetProgramBinary, GLuint(*this), bufsize, length, binaryFormat, binary);
}

void program::get_program_interface(GLenum programInterface, GLenum pname, GLint *params) const
{
	gl_call(glGetProgramInterfaceiv, GLuint(*this), programInterface, pname, params);
}

void program::get_program_resource(GLenum programInterface, GLuint index, GLsizei propCount,
								   const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params) const
{
	gl_call(glGetProgramResourceiv, GLuint(*this), programInterface, index, propCount, props,
			bufSize, length, params);
}

void program::get_program_resource_name(GLenum programInterface, GLuint index, GLsizei bufSize,
										GLsizei *length, char *name) const
{
	gl_call(glGetProgramResourceName, GLuint(*this), programInterface, index, bufSize, length, name);
}

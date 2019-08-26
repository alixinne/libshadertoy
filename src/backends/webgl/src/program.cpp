#include <utility>
#include <vector>

#include <GLES3/gl3.h>

#include "shadertoy/backends/webgl/program.hpp"
#include "shadertoy/backends/webgl/shader.hpp"
#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/backends/webgl/backend.hpp"

using namespace shadertoy::backends;
using namespace shadertoy::backends::webgl;

null_program_error::null_program_error()
: shadertoy_error("An attempt was made to dereference a null program")
{
}

attrib_location::attrib_location(GLint location) : location_(location) {}

void attrib_location::vertex_pointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride,
									 const GLvoid *pointer) const
{
	gl_call(glVertexAttribPointer, location_, size, type, normalized, stride, pointer);
}

void attrib_location::enable_vertex_array() const { gl_call(glEnableVertexAttribArray, location_); }

void uniform_location::use_program() const { backend::current().use_program(program_); }

uniform_location::uniform_location(const gx::program &program, GLint location)
: program_(GLuint(program)), location_(location)
{
}

bool uniform_location::is_active() const { return location_ != -1; }

void program::link() const
{
	gl_call(glLinkProgram, GLuint(*this));

	GLint linkStatus;
	gl_call(glGetProgramiv, GLuint(*this), GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		throw backends::gx::program_link_error(GLuint(*this), this->log());
	}

	validate();
}

void program::use() const { backend::current().use_program(GLuint(*this)); }

void program::validate() const
{
	gl_call(glValidateProgram, GLuint(*this));

	GLint validateStatus;
	gl_call(glGetProgramiv, GLuint(*this), GL_LINK_STATUS, &validateStatus);
	if (validateStatus != GL_TRUE)
	{
		throw backends::gx::program_validate_error(GLuint(*this), this->log());
	}
}

std::unique_ptr<gx::uniform_location> program::get_uniform_location(const GLchar *name) const
{
	GLint location = gl_call(glGetUniformLocation, GLuint(*this), name);
	return std::make_unique<webgl::uniform_location>(*this, location);
}

std::unique_ptr<gx::attrib_location> program::get_attrib_location(const GLchar *name) const
{
	GLint location = gl_call(glGetAttribLocation, GLuint(*this), name);
	return std::make_unique<webgl::attrib_location>(location);
}

void program::attach_shader(const gx::shader &shader) const
{
	gl_call(glAttachShader, GLuint(*this), GLuint(shader));
}

void program::detach_shader(const gx::shader &shader) const
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

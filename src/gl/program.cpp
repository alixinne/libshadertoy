#include <vector>

#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/shader.hpp"
#include "shadertoy/gl/program.hpp"

using namespace shadertoy::gl;

null_program_error::null_program_error()
	: shadertoy_error("An attempt was made to dereference a null program")
{
}

attrib_location::attrib_location(const program &program, GLint location)
	: program_(GLuint(program)),
	location_(location)
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

bool uniform_location::set_value(const GLint &v0) const
{
	if (is_active())
	{
		gl_call(glProgramUniform1i, program_, location_, v0);
		return true;
	}

	return false;
}

bool uniform_location::set_value(const GLfloat &v0) const
{
	if (is_active())
	{
		gl_call(glProgramUniform1f, program_, location_, v0);
		return true;
	}

	return false;
}

bool uniform_location::set_value(const glm::vec2 &v) const
{
	if (is_active())
	{
		gl_call(glProgramUniform2f, program_, location_, v.x, v.y);
		return true;
	}

	return false;
}

bool uniform_location::set_value(const glm::vec3 &v) const
{
	if (is_active())
	{
		gl_call(glProgramUniform3f, program_, location_, v.x, v.y, v.z);
		return true;
	}

	return false;
}

bool uniform_location::set_value(const glm::vec4 &v) const
{
	if (is_active())
	{
		gl_call(glProgramUniform4f, program_, location_, v.x, v.y, v.z, v.w);
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const GLint *v0) const
{
	if (is_active())
	{
		gl_call(glProgramUniform1iv, program_, location_, count, v0);
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const GLfloat *v0) const
{
	if (is_active())
	{
		gl_call(glProgramUniform1fv, program_, location_, count, v0);
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const glm::vec2 *v) const
{
	if (is_active())
	{
		gl_call(glProgramUniform2fv, program_, location_, count, reinterpret_cast<const GLfloat*>(v));
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const glm::vec3 *v) const
{
	if (is_active())
	{
		gl_call(glProgramUniform3fv, program_, location_, count, reinterpret_cast<const GLfloat*>(v));
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const glm::vec4 *v) const
{
	if (is_active())
	{
		gl_call(glProgramUniform4fv, program_, location_, count, reinterpret_cast<const GLfloat*>(v));
		return true;
	}

	return false;
}

program_link_error::program_link_error(GLuint programId, const std::string &log)
	: shadertoy_error("OpenGL program linking error"),
	program_id_(programId),
	log_(log)
{
}

program_validate_error::program_validate_error(GLuint programId, const std::string &log)
	: shadertoy_error("OpenGL program validation error"),
	program_id_(programId),
	log_(log)
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
	return uniform_location(*this, location);
}

attrib_location program::get_attrib_location(const GLchar *name) const
{
	GLint location = gl_call(glGetAttribLocation, GLuint(*this), name);
	return attrib_location(*this, location);
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
		return std::string();

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

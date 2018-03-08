#include <vector>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/shader.hpp"
#include "shadertoy/gl/program.hpp"

using namespace shadertoy::gl;

null_program_error::null_program_error()
	: shadertoy_error("An attempt was made to dereference a null program")
{
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

bool uniform_location::set_value(const GLint &v0)
{
	if (is_active())
	{
		gl_call(glProgramUniform1i, program_, location_, v0);
		return true;
	}

	return false;
}

bool uniform_location::set_value(const GLfloat &v0)
{
	if (is_active())
	{
		gl_call(glProgramUniform1f, program_, location_, v0);
		return true;
	}

	return false;
}

bool uniform_location::set_value(const glm::vec2 &v)
{
	if (is_active())
	{
		gl_call(glProgramUniform2f, program_, location_, v.x, v.y);
		return true;
	}

	return false;
}

bool uniform_location::set_value(const glm::vec3 &v)
{
	if (is_active())
	{
		gl_call(glProgramUniform3f, program_, location_, v.x, v.y, v.z);
		return true;
	}

	return false;
}

bool uniform_location::set_value(const glm::vec4 &v)
{
	if (is_active())
	{
		gl_call(glProgramUniform4f, program_, location_, v.x, v.y, v.z, v.w);
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const GLint *v0)
{
	if (is_active())
	{
		gl_call(glProgramUniform1iv, program_, location_, count, v0);
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const GLfloat *v0)
{
	if (is_active())
	{
		gl_call(glProgramUniform1fv, program_, location_, count, v0);
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const glm::vec2 *v)
{
	if (is_active())
	{
		gl_call(glProgramUniform2fv, program_, location_, count, reinterpret_cast<const GLfloat*>(v));
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const glm::vec3 *v)
{
	if (is_active())
	{
		gl_call(glProgramUniform3fv, program_, location_, count, reinterpret_cast<const GLfloat*>(v));
		return true;
	}

	return false;
}

bool uniform_location::set_value(size_t count, const glm::vec4 *v)
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

void program::link()
{
	gl_call(glLinkProgram, GLuint(*this));

	GLint linkStatus;
	gl_call(glGetProgramiv, GLuint(*this), GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		throw program_link_error(GLuint(*this), this->log());
	}
}

void program::use()
{
	gl_call(glUseProgram, GLuint(*this));
}

void program::validate()
{
	gl_call(glValidateProgram, GLuint(*this));

	GLint validateStatus;
	gl_call(glGetProgramiv, GLuint(*this), GL_LINK_STATUS, &validateStatus);
	if (validateStatus != GL_TRUE)
	{
		throw program_validate_error(GLuint(*this), this->log());
	}
}

uniform_location program::get_uniform_location(const GLchar *name)
{
	GLint location = gl_call(glGetUniformLocation, GLuint(*this), name);
	return uniform_location(*this, location);
}

void program::attach_shader(const shader &shader)
{
	gl_call(glAttachShader, GLuint(*this), GLuint(shader));
}

std::string program::log()
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

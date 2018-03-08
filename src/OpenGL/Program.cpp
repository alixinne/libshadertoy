#include <vector>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Shader.hpp"
#include "shadertoy/OpenGL/Program.hpp"

using namespace shadertoy::OpenGL;

NullProgramError::NullProgramError()
	: ShadertoyError("An attempt was made to dereference a null program")
{
}

UniformLocation::UniformLocation(const Program &program, GLint location)
	: _program(GLuint(program)),
	_location(location)
{

}

bool UniformLocation::IsActive() const
{
	return _location != -1;
}

bool UniformLocation::SetValue(const GLint &v0)
{
	if (IsActive())
	{
		glCall(glProgramUniform1i, _program, _location, v0);
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(const GLfloat &v0)
{
	if (IsActive())
	{
		glCall(glProgramUniform1f, _program, _location, v0);
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(const glm::vec2 &v)
{
	if (IsActive())
	{
		glCall(glProgramUniform2f, _program, _location, v.x, v.y);
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(const glm::vec3 &v)
{
	if (IsActive())
	{
		glCall(glProgramUniform3f, _program, _location, v.x, v.y, v.z);
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(const glm::vec4 &v)
{
	if (IsActive())
	{
		glCall(glProgramUniform4f, _program, _location, v.x, v.y, v.z, v.w);
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(size_t count, const GLint *v0)
{
	if (IsActive())
	{
		glCall(glProgramUniform1iv, _program, _location, count, v0);
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(size_t count, const GLfloat *v0)
{
	if (IsActive())
	{
		glCall(glProgramUniform1fv, _program, _location, count, v0);
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(size_t count, const glm::vec2 *v)
{
	if (IsActive())
	{
		glCall(glProgramUniform2fv, _program, _location, count, reinterpret_cast<const GLfloat*>(v));
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(size_t count, const glm::vec3 *v)
{
	if (IsActive())
	{
		glCall(glProgramUniform3fv, _program, _location, count, reinterpret_cast<const GLfloat*>(v));
		return true;
	}

	return false;
}

bool UniformLocation::SetValue(size_t count, const glm::vec4 *v)
{
	if (IsActive())
	{
		glCall(glProgramUniform4fv, _program, _location, count, reinterpret_cast<const GLfloat*>(v));
		return true;
	}

	return false;
}

ProgramLinkError::ProgramLinkError(GLuint programId, const std::string &log)
	: ShadertoyError("OpenGL program linking error"),
	_programId(programId),
	_log(log)
{
}

ProgramValidateError::ProgramValidateError(GLuint programId, const std::string &log)
	: ShadertoyError("OpenGL program validation error"),
	_programId(programId),
	_log(log)
{
}

void Program::Link()
{
	glCall(glLinkProgram, GLuint(*this));

	GLint linkStatus;
	glCall(glGetProgramiv, GLuint(*this), GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		throw ProgramLinkError(GLuint(*this), this->Log());
	}
}

void Program::Use()
{
	glCall(glUseProgram, GLuint(*this));
}

void Program::Validate()
{
	glCall(glValidateProgram, GLuint(*this));

	GLint validateStatus;
	glCall(glGetProgramiv, GLuint(*this), GL_LINK_STATUS, &validateStatus);
	if (validateStatus != GL_TRUE)
	{
		throw ProgramValidateError(GLuint(*this), this->Log());
	}
}

UniformLocation Program::GetUniformLocation(const GLchar *name)
{
	GLint location = glCall(glGetUniformLocation, GLuint(*this), name);
	return UniformLocation(*this, location);
}

void Program::AttachShader(const Shader &shader)
{
	glCall(glAttachShader, GLuint(*this), GLuint(shader));
}

std::string Program::Log()
{
	// Get log length
	GLint infoLogLength = 0;
	glCall(glGetProgramiv, GLuint(*this), GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength == 0)
		return std::string();

	// Get log
	std::vector<GLchar> logStr(infoLogLength);
	glCall(glGetProgramInfoLog, GLuint(*this), infoLogLength, nullptr, logStr.data());

	// exclude the null character from the range passed to string constructor
	return std::string(logStr.begin(), logStr.end() - 1);
}

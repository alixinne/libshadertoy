#include "stdafx.hpp"
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

void Program::Link()
{
	glCall(glLinkProgram, GLuint(*this));
}

void Program::Use()
{
	glCall(glUseProgram, GLuint(*this));
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

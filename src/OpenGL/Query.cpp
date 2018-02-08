#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Query.hpp"

using namespace shadertoy::OpenGL;

NullQueryError::NullQueryError()
	: ShadertoyError("An attempt was made to dereference a null query")
{
}

void Query::Begin(GLenum target)
{
	glCall(glBeginQuery, target, GLuint(*this));
}

void Query::End(GLenum target)
{
	glCall(glEndQuery, target);
}

void Query::GetObjectiv(GLenum pname, GLint *params)
{
	glCall(glGetQueryObjectiv, GLuint(*this), pname, params);
}

void Query::GetObjectui64v(GLenum pname, GLuint64 *params)
{
	glCall(glGetQueryObjectui64v, GLuint(*this), pname, params);
}

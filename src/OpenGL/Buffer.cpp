#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Buffer.hpp"

using namespace shadertoy::OpenGL;

NullBufferError::NullBufferError()
	: ShadertoyError("An attempt was made to dereference a null buffer")
{
}

void Buffer::Bind(GLenum target)
{
	glCall(glBindBuffer, target, GLuint(*this));
}

void Buffer::Data(GLsizei size, const void *data, GLenum usage)
{
	glCall(glNamedBufferData, GLuint(*this), size, data, usage);
}

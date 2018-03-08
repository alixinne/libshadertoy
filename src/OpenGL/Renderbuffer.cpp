#include <epoxy/gl.h>

#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Renderbuffer.hpp"

using namespace shadertoy::OpenGL;

NullRenderbufferError::NullRenderbufferError()
	: ShadertoyError("An attempt was made to dereference a null renderbuffer")
{
}

void Renderbuffer::Bind(GLenum target)
{
	glCall(glBindRenderbuffer, target, GLuint(*this));
}

void Renderbuffer::Storage(GLenum internalFormat, GLsizei width, GLsizei height)
{
	glCall(glNamedRenderbufferStorage, GLuint(*this), internalFormat, width, height);
}

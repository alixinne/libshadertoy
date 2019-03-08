#include <epoxy/gl.h>

#include "shadertoy/gl/renderbuffer.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::gl;

null_renderbuffer_error::null_renderbuffer_error()
	: shadertoy_error("An attempt was made to dereference a null renderbuffer")
{
}

void renderbuffer::bind(GLenum target) const
{
	gl_call(glBindRenderbuffer, target, GLuint(*this));
}

void renderbuffer::unbind(GLenum target) const
{
	gl_call(glBindRenderbuffer, target, 0);
}

void renderbuffer::storage(GLenum internalFormat, GLsizei width, GLsizei height) const
{
	gl_call(glNamedRenderbufferStorage, GLuint(*this), internalFormat, width, height);
}

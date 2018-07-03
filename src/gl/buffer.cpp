#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/buffer.hpp"

using namespace shadertoy::gl;

null_buffer_error::null_buffer_error()
	: shadertoy_error("An attempt was made to dereference a null buffer")
{
}

void buffer::bind(GLenum target) const
{
	gl_call(glBindBuffer, target, GLuint(*this));
}

void buffer::unbind(GLenum target) const
{
	gl_call(glBindBuffer, target, 0);
}

void buffer::data(GLsizei size, const void *data, GLenum usage) const
{
	gl_call(glNamedBufferData, GLuint(*this), size, data, usage);
}

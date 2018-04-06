#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/buffer.hpp"

using namespace shadertoy::gl;

null_buffer_error::null_buffer_error()
	: shadertoy_error("An attempt was made to dereference a null buffer")
{
}

void buffer::bind(GLenum target)
{
	gl_call(glBindBuffer, target, GLuint(*this));
}

void buffer::data(GLsizei size, const void *data, GLenum usage)
{
	gl_call(glNamedBufferData, GLuint(*this), size, data, usage);
}
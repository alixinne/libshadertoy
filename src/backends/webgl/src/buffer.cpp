#include <GLES3/gl3.h>

#include "shadertoy/backends/webgl/buffer.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::backends::webgl;

null_buffer_error::null_buffer_error()
: shadertoy_error("An attempt was made to dereference a null buffer")
{
}

buffer::buffer(GLenum target) : resource(), target_(target)
{}

void buffer::bind(GLenum target) const { gl_call(glBindBuffer, target, GLuint(*this)); }

void buffer::unbind(GLenum target) const { gl_call(glBindBuffer, target, 0); }

void buffer::data(GLsizei size, const void *data, GLenum usage) const
{
	gl_call(glBindBuffer, target_, GLuint(*this));
	gl_call(glBufferData, target_, size, data, usage);
}

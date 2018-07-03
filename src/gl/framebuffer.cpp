#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/texture.hpp"
#include "shadertoy/gl/framebuffer.hpp"

using namespace shadertoy::gl;

null_framebuffer_error::null_framebuffer_error()
	: shadertoy_error("An attempt was made to dereference a null framebuffer")
{
}

void framebuffer::bind(GLenum target) const
{
	gl_call(glBindFramebuffer, target, GLuint(*this));
}

void framebuffer::unbind(GLenum target) const
{
	gl_call(glBindFramebuffer, target, 0);
}

void framebuffer::texture(GLenum attachment, shadertoy::gl::texture &texture, GLint level) const
{
	gl_call(glNamedFramebufferTexture, GLuint(*this), attachment, GLuint(texture), level);
}

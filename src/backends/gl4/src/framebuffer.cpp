#include <epoxy/gl.h>

#include "shadertoy/backends/gl4/framebuffer.hpp"
#include "shadertoy/backends/gl4/renderbuffer.hpp"
#include "shadertoy/backends/gl4/texture.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::backends::gl4;

null_framebuffer_error::null_framebuffer_error()
: shadertoy_error("An attempt was made to dereference a null framebuffer")
{
}

void framebuffer::bind(GLenum target) const { gl_call(glBindFramebuffer, target, GLuint(*this)); }

void framebuffer::unbind(GLenum target) const { gl_call(glBindFramebuffer, target, 0); }

void framebuffer::texture(GLenum attachment, const gx::texture &texture, GLint level) const
{
	gl_call(glNamedFramebufferTexture, GLuint(*this), attachment, GLuint(texture), level);
}

void framebuffer::framebuffer_renderbuffer(GLenum attachment, GLenum renderbuffertarget,
										   const gx::renderbuffer &renderbuffer) const
{
	gl_call(glNamedFramebufferRenderbuffer, GLuint(*this), attachment, renderbuffertarget, GLuint(renderbuffer));
}

void framebuffer::texture_2d(GLenum target, GLenum attachment, GLenum texture_target,
							 const gx::texture &texture, GLint level) const
{
	gl_call(glFramebufferTexture2D, target, attachment, texture_target, GLuint(texture), level);
}

void framebuffer::draw_buffers(GLsizei n, const GLenum *bufs) const
{
	gl_call(glNamedFramebufferDrawBuffers, GLuint(*this), n, bufs);
}

void framebuffer::get_viewport(GLint viewport[4]) const
{
	glGetIntegerv(GL_VIEWPORT, std::addressof(viewport[0]));
}

void framebuffer::set_viewport(GLint x, GLint y, GLsizei width, GLsizei height) const
{
	gl_call(glViewport, x, y, width, height);
}

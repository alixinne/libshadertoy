#include <GLES3/gl3.h>

#include "shadertoy/backends/webgl/framebuffer.hpp"
#include "shadertoy/backends/webgl/renderbuffer.hpp"
#include "shadertoy/backends/webgl/texture.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::backends;
using namespace shadertoy::backends::webgl;

null_framebuffer_error::null_framebuffer_error()
: shadertoy_error("An attempt was made to dereference a null framebuffer")
{
}

void framebuffer::bind(GLenum target) const { gl_call(glBindFramebuffer, target, GLuint(*this)); }

void framebuffer::unbind(GLenum target) const { gl_call(glBindFramebuffer, target, 0); }

void framebuffer::texture(GLenum attachment, const gx::texture &texture, GLint level) const
{
	gl_call(glBindFramebuffer, GL_FRAMEBUFFER, GLuint(*this));
	gl_call(glFramebufferTexture2D, GL_FRAMEBUFFER, attachment,
			reinterpret_cast<const webgl::texture *>(&texture)->target(), GLuint(texture), level);
}

void framebuffer::framebuffer_renderbuffer(GLenum attachment, GLenum renderbuffertarget,
										   const gx::renderbuffer &renderbuffer) const
{
	gl_call(glBindFramebuffer, GL_FRAMEBUFFER, GLuint(*this));
	gl_call(glBindRenderbuffer, GL_RENDERBUFFER, GLuint(renderbuffer));
	gl_call(glFramebufferRenderbuffer, GL_FRAMEBUFFER, attachment, renderbuffertarget, GLuint(renderbuffer));
}

void framebuffer::texture_2d(GLenum target, GLenum attachment, GLenum texture_target,
							 const gx::texture &texture, GLint level) const
{
	gl_call(glFramebufferTexture2D, target, attachment, texture_target, GLuint(texture), level);
}

void framebuffer::draw_buffers(GLsizei n, const GLenum *bufs) const
{
	gl_call(glBindFramebuffer, GL_FRAMEBUFFER, GLuint(*this));
	gl_call(glDrawBuffers, n, bufs);
}

void framebuffer::get_viewport(GLint viewport[4]) const
{
	glGetIntegerv(GL_VIEWPORT, std::addressof(viewport[0]));
}

void framebuffer::set_viewport(GLint x, GLint y, GLsizei width, GLsizei height) const
{
	gl_call(glViewport, x, y, width, height);
}

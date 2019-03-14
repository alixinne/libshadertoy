#include <epoxy/gl.h>

#include "shadertoy/gl/framebuffer.hpp"
#include "shadertoy/gl/renderbuffer.hpp"
#include "shadertoy/gl/texture.hpp"
#include "shadertoy/shadertoy_error.hpp"

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

void framebuffer::texture(GLenum attachment, const shadertoy::gl::texture &texture, GLint level) const
{
	gl_call(glNamedFramebufferTexture, GLuint(*this), attachment, GLuint(texture), level);
}

void framebuffer::framebuffer_renderbuffer(GLenum attachment, GLenum renderbuffertarget, const renderbuffer &renderbuffer) const
{
	gl_call(glNamedFramebufferRenderbuffer, GLuint(*this), attachment, renderbuffertarget, GLuint(renderbuffer));
}

bound_ops<framebuffer>::bound_ops(const framebuffer &resource)
	: bound_ops_base<framebuffer>(resource)
{}

const bound_ops<framebuffer> &bound_ops<framebuffer>::texture_2d(GLenum target,
																 GLenum attachment,
																 GLenum texture_target,
																 const texture &texture,
																 GLint level) const
{
	gl_call(glFramebufferTexture2D,
			target,
			attachment,
			texture_target,
			GLuint(texture),
			level);
	
	return (*this);
}

const bound_ops<framebuffer> &bound_ops<framebuffer>::draw_buffers(GLsizei n, const GLenum *bufs) const
{
	gl_call(glDrawBuffers, n, bufs);

	return (*this);
}

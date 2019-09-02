#ifndef _SHADERTOY_BACKENDS_GX_FRAMEBUFFER_HPP_
#define _SHADERTOY_BACKENDS_GX_FRAMEBUFFER_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

#include "shadertoy/backends/gx/renderbuffer.hpp"
#include "shadertoy/backends/gx/texture.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Represents an OpenGL framebuffer object
 */
class stbackend_gx_EXPORT framebuffer : public resource
{
	public:
	/**
	 * @brief glBindFramebuffer
	 *
	 * @param target Target to bind this framebuffer to
	 *
	 * @throws opengl_error
	 * @throws null_framebuffer_error
	 */
	virtual void bind(GLenum target) const = 0;

	/**
	 * @brief glBindFramebuffer
	 *
	 * Unbinds the current framebuffer.
	 *
	 * @param target Target to unbind the current framebuffer from
	 *
	 * @throws opengl_error
	 */
	virtual void unbind(GLenum target) const = 0;

	/**
	 * @brief glNamedFramebufferTexture
	 *
	 * @param attachment Framebuffer attachment
	 * @param texture    Texture object to attach
	 * @param level      Texture level to attach
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void texture(GLenum attachment, const texture &texture, GLint level) const = 0;

	/**
	 * @brief glNamedFramebufferRenderbuffer
	 *
	 * @param attachment         Framebuffer attachment
	 * @param renderbuffertarget Renderbuffer target
	 * @param renderbuffer       Renderbuffer
	 */
	virtual void framebuffer_renderbuffer(GLenum attachment, GLenum renderbuffertarget,
										  const renderbuffer &renderbuffer) const = 0;

	/**
	 * @brief glFramebufferTexture2D
	 *
	 * Note: requires binding (non-DSA).
	 *
	 * @param target
	 * @param attachment
	 * @param texture_target
	 * @param texture
	 * @param level
	 */
	virtual void texture_2d(GLenum target, GLenum attachment, GLenum texture_target,
							const class texture &texture, GLint level) const = 0;

	/**
	 * @brief glNamedFramebufferDrawBuffers
	 *
	 * @param n
	 * @param bufs
	 */
	virtual void draw_buffers(GLsizei n, const GLenum *bufs) const = 0;

	virtual void get_viewport(GLint viewport[4]) const = 0;

	virtual void set_viewport(GLint x, GLint y, GLsizei width, GLsizei height) const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_FRAMEBUFFER_HPP_ */

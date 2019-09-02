#ifndef _SHADERTOY_BACKENDS_GL4_FRAMEBUFFER_HPP_
#define _SHADERTOY_BACKENDS_GL4_FRAMEBUFFER_HPP_

#include "shadertoy/backends/gl4/pre.hpp"
#include "shadertoy/backends/gl4/resource.hpp"
#include "shadertoy/backends/gx/framebuffer.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an attempt is made to dereference a null framebuffer.
 */
class stbackend_gl4_EXPORT null_framebuffer_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the null_framebuffer_error class.
	 */
	explicit null_framebuffer_error();
};

/**
 * @brief Represents an OpenGL framebuffer object
 */
class stbackend_gl4_EXPORT framebuffer
: public resource<framebuffer, multi_allocator<&glCreateFramebuffers, &glDeleteFramebuffers>, null_framebuffer_error, gx::framebuffer>
{
	public:
	framebuffer() : resource() {}
	framebuffer(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	/**
	 * @brief glBindFramebuffer
	 *
	 * @param target Target to bind this framebuffer to
	 *
	 * @throws opengl_error
	 * @throws null_framebuffer_error
	 */
	void bind(GLenum target) const override;

	/**
	 * @brief glBindFramebuffer
	 *
	 * Unbinds the current framebuffer.
	 *
	 * @param target Target to unbind the current framebuffer from
	 *
	 * @throws opengl_error
	 */
	void unbind(GLenum target) const override;

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
	void texture(GLenum attachment, const gx::texture &texture, GLint level) const override;

	/**
	 * @brief glNamedFramebufferRenderbuffer
	 *
	 * @param attachment         Framebuffer attachment
	 * @param renderbuffertarget Renderbuffer target
	 * @param renderbuffer       Renderbuffer
	 */
	void framebuffer_renderbuffer(GLenum attachment, GLenum renderbuffertarget,
								  const gx::renderbuffer &renderbuffer) const override;

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
	void texture_2d(GLenum target, GLenum attachment, GLenum texture_target,
					const gx::texture &texture, GLint level) const override;

	/**
	 * @brief glNamedFramebufferDrawBuffers
	 *
	 * @param n
	 * @param bufs
	 */
	void draw_buffers(GLsizei n, const GLenum *bufs) const override;

	void get_viewport(GLint viewport[4]) const override;

	void set_viewport(GLint x, GLint y, GLsizei width, GLsizei height) const override;
};
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_FRAMEBUFFER_HPP_ */

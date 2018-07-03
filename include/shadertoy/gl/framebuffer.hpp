#ifndef _SHADERTOY_GL_FRAMEBUFFER_HPP_
#define _SHADERTOY_GL_FRAMEBUFFER_HPP_

#include "shadertoy/gl/resource.hpp"

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null framebuffer.
	 */
		class shadertoy_EXPORT null_framebuffer_error : public shadertoy::shadertoy_error
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
	class shadertoy_EXPORT framebuffer : public resource<
		framebuffer,
		multi_allocator<&glCreateFramebuffers, &glDeleteFramebuffers>,
		null_framebuffer_error>
	{
	public:
		framebuffer() : resource() {}
		framebuffer(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
		resource_type &operator=(resource_type &&other) { return assign_operator(std::forward<resource_type &&>(other)); }

		/**
		 * @brief glBindFramebuffer
		 *
		 * @param target Target to bind this framebuffer to
		 *
		 * @throws opengl_error
		 */
		void bind(GLenum target) const;

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
		void texture(GLenum attachment, texture &texture, GLint level) const;
	};
}
}

#endif /* _SHADERTOY_GL_FRAMEBUFFER_HPP_ */

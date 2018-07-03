#ifndef _SHADERTOY_GL_RENDERBUFFER_HPP_
#define _SHADERTOY_GL_RENDERBUFFER_HPP_

#include "shadertoy/gl/resource.hpp"

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null renderbuffer.
	 */
	class shadertoy_EXPORT null_renderbuffer_error : public shadertoy::shadertoy_error
	{
	public:
		explicit null_renderbuffer_error();
	};

	/**
	 * @brief Represents an OpenGL Renderbuffer object.
	 */
	class shadertoy_EXPORT renderbuffer : public resource<
		renderbuffer,
		multi_allocator<&glCreateRenderbuffers, &glDeleteRenderbuffers>,
		null_renderbuffer_error>
	{
	public:
		renderbuffer() : resource() {}
		renderbuffer(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
		resource_type &operator=(resource_type &&other) { return assign_operator(std::forward<resource_type &&>(other)); }

		/**
		 * @brief glBindRenderbuffer
		 *
		 * @param target Target to bind this renderbuffer to
		 *
		 * @throws opengl_error
		 * @throws null_renderbuffer_error
		 */
		void bind(GLenum target) const;

		// glNamedRenderbufferStorage
		/**
		 * @brief glNamedRenderbufferStorage
		 *
		 * @param internalFormat Internal format
		 * @param width          Width
		 * @param height         Height
		 *
		 * @throws opengl_error
		 * @throws null_renderbuffer_error
		 */
		void storage(GLenum internalFormat, GLsizei width, GLsizei height) const;
	};
}
}

#endif /* _SHADERTOY_GL_RENDERBUFFER_HPP_ */

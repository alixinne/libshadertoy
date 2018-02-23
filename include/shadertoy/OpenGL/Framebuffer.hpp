#ifndef _SHADERTOY_OPENGL_FRAMEBUFFER_HPP_
#define _SHADERTOY_OPENGL_FRAMEBUFFER_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null framebuffer.
	 */
	class shadertoy_EXPORT NullFramebufferError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the NullFramebufferError class.
		 */
		explicit NullFramebufferError();
	};

	/**
	 * @brief Represents an OpenGL framebuffer object
	 */
	class shadertoy_EXPORT Framebuffer : public Resource<
		Framebuffer,
		MultiAllocator<&glCreateFramebuffers, &glDeleteFramebuffers>,
		NullFramebufferError>
	{
	public:
		/**
		 * @brief glBindFramebuffer
		 *
		 * @param target Target to bind this framebuffer to
		 *
		 * @throws OpenGLError
		 */
		void Bind(GLenum target);

		/**
		 * @brief glNamedFramebufferTexture
		 *
		 * @param attachment Framebuffer attachment
		 * @param texture    Texture object to attach
		 * @param level      Texture level to attach
		 *
		 * @throws OpenGLError
		 * @throws NullTextureError
		 */
		void Texture(GLenum attachment, Texture &texture, GLint level);
	};
}
}

#endif /* _SHADERTOY_OPENGL_FRAMEBUFFER_HPP_ */

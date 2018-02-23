#ifndef _SHADERTOY_OPENGL_RENDERBUFFER_HPP_
#define _SHADERTOY_OPENGL_RENDERBUFFER_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null renderbuffer.
	 */
	class shadertoy_EXPORT NullRenderbufferError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullRenderbufferError();
	};

	/**
	 * @brief Represents an OpenGL Renderbuffer object.
	 */
	class shadertoy_EXPORT Renderbuffer : public Resource<
		Renderbuffer,
		MultiAllocator<&glCreateRenderbuffers, &glDeleteRenderbuffers>,
		NullRenderbufferError>
	{
	public:
		/**
		 * @brief glBindRenderbuffer
		 *
		 * @param target Target to bind this renderbuffer to
		 *
		 * @throws OpenGLError
		 * @throws NullRenderbufferError
		 */
		void Bind(GLenum target);

		// glNamedRenderbufferStorage
		/**
		 * @brief glNamedRenderbufferStorage
		 *
		 * @param internalFormat Internal format
		 * @param width          Width
		 * @param height         Height
		 *
		 * @throws OpenGLError
		 * @throws NullRenderbufferError
		 */
		void Storage(GLenum internalFormat, GLsizei width, GLsizei height);
	};
}
}

#endif /* _SHADERTOY_OPENGL_RENDERBUFFER_HPP_ */

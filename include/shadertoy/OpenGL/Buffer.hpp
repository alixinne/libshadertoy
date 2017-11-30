#ifndef _SHADERTOY_OPENGL_BUFFER_HPP_
#define _SHADERTOY_OPENGL_BUFFER_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * Error thrown when an attempt is made to obtain the id of a null buffer.
	 */
	class shadertoy_EXPORT NullBufferError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * Initializes a new instance of the NullBufferError class.
		 */
		explicit NullBufferError();
	};

	/**
	 * Represents an OpenGL buffer
	 */
	class shadertoy_EXPORT Buffer : public Resource<
		Buffer,
		MultiAllocator<&glCreateBuffers, &glDeleteBuffers>,
		NullBufferError>
	{
	public:
		/**
		 * glBindBuffer
		 *
		 * @param target Target to bind this buffer to
		 *
		 * @throws OpenGLError
		 * @throws NullBufferError
		 */
		void Bind(GLenum target);

		/**
		 * glNamedBufferData
		 * @param size  size of the data to send to the buffer
		 * @param data  pointer to the data to send to the buffer
		 * @param usage GL usage for this buffer
		 *
		 * @throws OpenGLError
		 * @throws NullBufferError
		 */
		void Data(GLsizei size, const void *data, GLenum usage);
	};
}
}

#endif /* _SHADERTOY_OPENGL_BUFFER_HPP_ */

#ifndef _SHADERTOY_GL_BUFFER_HPP_
#define _SHADERTOY_GL_BUFFER_HPP_

#include "shadertoy/gl/resource.hpp"

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Error thrown when an attempt is made to obtain the id of a null buffer.
	 */
	class shadertoy_EXPORT null_buffer_error : public shadertoy::shadertoy_error
	{
	public:
		/**
		 * @brief Initialize a new instance of the null_buffer_error class.
		 */
		explicit null_buffer_error();
	};

	/**
	 * @brief Represents an OpenGL buffer
	 */
	class shadertoy_EXPORT buffer : public resource<
		buffer,
		multi_allocator<&glCreateBuffers, &glDeleteBuffers>,
		null_buffer_error>
	{
	public:
		using base_resource_type::operator=;

		/**
		 * @brief glBindBuffer
		 *
		 * @param target Target to bind this buffer to
		 *
		 * @throws opengl_error
		 * @throws null_buffer_error
		 */
		void bind(GLenum target) const;

		/**
		 * @brief glNamedBufferData
		 * @param size  size of the data to send to the buffer
		 * @param data  pointer to the data to send to the buffer
		 * @param usage GL usage for this buffer
		 *
		 * @throws opengl_error
		 * @throws null_buffer_error
		 */
		void data(GLsizei size, const void *data, GLenum usage) const;
	};
}
}

#endif /* _SHADERTOY_GL_BUFFER_HPP_ */

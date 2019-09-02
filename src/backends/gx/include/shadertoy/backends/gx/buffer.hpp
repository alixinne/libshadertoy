#ifndef _SHADERTOY_BACKENDS_GX_BUFFER_HPP_
#define _SHADERTOY_BACKENDS_GX_BUFFER_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Represents an OpenGL buffer
 */
class stbackend_gx_EXPORT buffer : public resource
{
	public:
	/**
	 * @brief glBindBuffer
	 *
	 * @param target Target to bind this buffer to
	 */
	virtual void bind(GLenum target) const = 0;

	/**
	 * @brief glBindBuffer
	 *
	 * Unbinds the current buffer.
	 *
	 * @param target Target to unbind the current buffer from
	 */
	virtual void unbind(GLenum target) const = 0;

	/**
	 * @brief glNamedBufferData
	 * @param size  size of the data to send to the buffer
	 * @param data  pointer to the data to send to the buffer
	 * @param usage GL usage for this buffer
	 *
	 * @throws opengl_error
	 * @throws null_buffer_error
	 */
	virtual void data(GLsizei size, const void *data, GLenum usage) const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_BUFFER_HPP_ */

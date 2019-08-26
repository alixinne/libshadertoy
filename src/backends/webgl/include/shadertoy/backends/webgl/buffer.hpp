#ifndef _SHADERTOY_BACKENDS_WEBGL_BUFFER_HPP_
#define _SHADERTOY_BACKENDS_WEBGL_BUFFER_HPP_

#include "shadertoy/backends/webgl/pre.hpp"
#include "shadertoy/backends/webgl/resource.hpp"
#include "shadertoy/backends/gx/buffer.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_WEBGL_NAMESPACE_BEGIN
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
class shadertoy_EXPORT buffer
: public resource<buffer, multi_allocator<&glGenBuffers, &glDeleteBuffers>, null_buffer_error, gx::buffer>
{
	mutable GLenum target_;

	public:
	buffer(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	buffer(GLenum target);

	inline GLenum target() const
	{ return target_; }

	/**
	 * @brief glBindBuffer
	 *
	 * @param target Target to bind this buffer to
	 *
	 * @throws opengl_error
	 * @throws null_buffer_error
	 */
	void bind(GLenum target) const override;

	/**
	 * @brief glBindBuffer
	 *
	 * Unbinds the current buffer.
	 *
	 * @param target Target to unbind the current buffer frmo
	 *
	 * @throws opengl_error
	 */
	void unbind(GLenum target) const override;

	/**
	 * @brief glBufferData
	 *
	 * @param size   size of the data to send to the buffer
	 * @param data   pointer to the data to send to the buffer
	 * @param usage  GL usage for this buffer
	 *
	 * @throws opengl_error
	 * @throws null_buffer_error
	 */
	void data(GLsizei size, const void *data, GLenum usage) const override;
};
SHADERTOY_BACKENDS_WEBGL_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_WEBGL_BUFFER_HPP_ */

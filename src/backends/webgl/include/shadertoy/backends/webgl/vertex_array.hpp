#ifndef _SHADERTOY_BACKENDS_WEBGL_VERTEX_ARRAY_HPP_
#define _SHADERTOY_BACKENDS_WEBGL_VERTEX_ARRAY_HPP_

#include "shadertoy/backends/webgl/pre.hpp"
#include "shadertoy/backends/webgl/resource.hpp"
#include "shadertoy/backends/gx/vertex_array.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_WEBGL_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an attempt is made to dereference a null vertex_array.
 */
class shadertoy_EXPORT null_vertex_array_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the null_vertex_array_error class.
	 */
	explicit null_vertex_array_error();
};

/**
 * @brief Represents an OpenGL vertex array object
 */
class shadertoy_EXPORT vertex_array
: public resource<vertex_array, multi_allocator<&glGenVertexArrays, &glDeleteVertexArrays>, null_vertex_array_error, gx::vertex_array>
{
	public:
	vertex_array() : resource() {}
	vertex_array(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	/**
	 * @brief glBindVertexArray
	 *
	 * @throws null_vertex_array_error
	 */
	void bind() const override;

	/**
	 * @brief glBindVertexArray
	 *
	 * Unbinds the currently bound vertex_array
	 */
	void unbind() const override;

	/**
	 * @brief glDrawElements
	 */
	void draw_elements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) const override;
};
SHADERTOY_BACKENDS_WEBGL_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_WEBGL_VERTEX_ARRAY_HPP_ */

#ifndef _SHADERTOY_BACKENDS_GX_VERTEX_ARRAY_HPP_
#define _SHADERTOY_BACKENDS_GX_VERTEX_ARRAY_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Represents an OpenGL vertex array object
 */
class shadertoy_EXPORT vertex_array : public resource
{
	public:
	/**
	 * @brief glBindVertexArray
	 *
	 * @throws null_vertex_array_error
	 */
	virtual void bind() const = 0;

	/**
	 * @brief glBindVertexArray
	 *
	 * Unbinds the currently bound vertex_array
	 */
	virtual void unbind() const = 0;

	/**
	 * @brief glDrawElements
	 *
	 * Since glDrawElements requires state to be bound, this method
	 * must also bind the current vertex_array object.
	 */
	virtual void draw_elements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_VERTEX_ARRAY_HPP_ */

#ifndef _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_
#define _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{
namespace geometry
{

/**
 * @brief Represents the geometry and bindings for a screen quad
 */
class shadertoy_EXPORT screen_quad
{
	/// Vertex array object
	gl::vertex_array quad_array_;
	/// Vertex buffer for screen quad
	gl::buffer quad_corners_;
	/// Index buffer for screen quad
	gl::buffer quad_indices_;

public:
	/**
	 * @brief Initialize the screen quad geometry GL objects
	 */
	screen_quad();

	/**
	 * @brief Get the VAO to bind for rendering the geometry
	 *
	 * @return Reference to the VAO to bind to
	 */
	inline const gl::vertex_array &vertex_array() const
	{ return quad_array_; }

	/**
	 * @brief Renders the geometry assuming the VAO returned by screen_quad#vertex_array
	 *        is currently bound.
	 */
	void draw() const;
};
}
}

#endif /* _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_ */

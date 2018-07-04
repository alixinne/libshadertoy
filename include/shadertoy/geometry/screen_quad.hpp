#ifndef _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_
#define _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/geometry/basic_geometry.hpp"

namespace shadertoy
{
namespace geometry
{

/**
 * @brief Represents the geometry and bindings for a screen quad
 */
class shadertoy_EXPORT screen_quad : public basic_geometry
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

	inline const gl::vertex_array &vertex_array() const final
	{ return quad_array_; }

	void draw() const final;
};
}
}

#endif /* _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_ */

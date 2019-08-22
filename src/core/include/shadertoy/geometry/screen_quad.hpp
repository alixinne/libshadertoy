#ifndef _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_
#define _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/backends/gx/buffer.hpp"
#include "shadertoy/backends/gx/vertex_array.hpp"

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
	std::unique_ptr<backends::gx::vertex_array> quad_array_;
	/// Vertex buffer for screen quad
	std::unique_ptr<backends::gx::buffer> quad_corners_;
	/// Index buffer for screen quad
	std::unique_ptr<backends::gx::buffer> quad_indices_;

	public:
	/**
	 * @brief Initialize the screen quad geometry GL objects
	 */
	screen_quad();

	void draw() const final;
};
} // namespace geometry
} // namespace shadertoy

#endif /* _SHADERTOY_GEOMETRY_SCREEN_QUAD_HPP_ */

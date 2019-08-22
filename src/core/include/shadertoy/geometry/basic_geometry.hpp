#ifndef _SHADERTOY_GEOMETRY_BASIC_GEOMETRY_HPP_
#define _SHADERTOY_GEOMETRY_BASIC_GEOMETRY_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/backends/gx/query.hpp"
#include "shadertoy/backends/gx/vertex_array.hpp"

namespace shadertoy
{
namespace geometry
{

/**
 * @brief Base class for geometry that can be rendered
 */
class shadertoy_EXPORT basic_geometry
{
	public:
	virtual ~basic_geometry() = default;

	/**
	 * @brief Renders the geometry
	 */
	virtual void draw() const = 0;

	/**
	 * @brief Renders the geometry by binding the VAO and calling the draw method
	 */
	void render() const;

	/**
	 * @brief Renders the geometry by binding the VAO and calling the draw method
	 *
	 * @param timer_query Timer query object to use to measure the performance of the draw call
	 */
	void render(const backends::gx::query &timer_query) const;
};
} // namespace geometry
} // namespace shadertoy

#endif

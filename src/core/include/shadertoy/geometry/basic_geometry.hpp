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
	 * @brief Get the VAO to bind for rendering the geometry
	 *
	 * @return Reference to the VAO to bind to
	 */
	virtual const backends::gx::vertex_array &vertex_array() const = 0;

	/**
	 * @brief Renders the geometry assuming the VAO returned by screen_quad#vertex_array
	 *        is currently bound.
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
}
}

#endif

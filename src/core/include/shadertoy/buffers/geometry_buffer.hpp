#ifndef _SHADERTOY_BUFFERS_GEOMETRY_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_GEOMETRY_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/program_buffer.hpp"

#include "shadertoy/geometry/basic_geometry.hpp"

namespace shadertoy
{
namespace buffers
{

/**
 * @brief      Represents a buffer that operates on the given geometry
 */
class shadertoy_EXPORT geometry_buffer : public program_buffer
{
	/// Geometry object
	std::weak_ptr<geometry::basic_geometry> geometry_;

	protected:
	void init_geometry(const render_context &context, const io_resource &io) override;

	void render_geometry(const render_context &context, const io_resource &io) override;

	public:
#if SHADERTOY_HAS_PROGRAM_INTERFACE
	/**
	 * @brief      initialize a new geometry buffer
	 *
	 * @param[in]  id       identifier for this buffer
	 */
	geometry_buffer(const std::string &id);
#else
	/**
	 * @brief      initialize a new geometry buffer
	 *
	 * @param[in]  id       identifier for this buffer
	 * @param[in]  outputs  Number of outputs for this buffer
	 */
	geometry_buffer(const std::string &id, size_t outputs = 1);
#endif

	/**
	 * @brief      Get the current geometry object
	 *
	 * @return     Pointer to the current geometry object
	 */
	inline const std::weak_ptr<geometry::basic_geometry> &geometry() const { return geometry_; }

	/**
	 * @brief      Sets the geometry object to use for rendering
	 *
	 * @param      new_geometry Pointer to the new geometry object
	 */
	inline void geometry(std::weak_ptr<geometry::basic_geometry> new_geometry)
	{
		geometry_ = new_geometry;
	}
};
} // namespace buffers
} // namespace shadertoy

#endif /* _SHADERTOY_BUFFERS_GEOMETRY_BUFFER_HPP_ */

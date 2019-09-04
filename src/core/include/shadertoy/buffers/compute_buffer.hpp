#ifndef _SHADERTOY_BUFFERS_COMPUTE_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_COMPUTE_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#if SHADERTOY_HAS_COMPUTE
#include "shadertoy/buffers/basic_buffer.hpp"

#include "shadertoy/program_host.hpp"

namespace shadertoy
{
namespace buffers
{

/**
 * @brief Represents a buffer backed by a compute shader
 */
class shadertoy_EXPORT compute_buffer : public basic_buffer
{
	/// OpenGL program host
	program_host host_;

	/// Work group counts
	glm::uvec3 num_groups_;

	protected:
	/**
	 * @brief Dispatch the compute operation
	 */
	virtual void dispatch_compute();

	public:
	/**
	 * @brief      Initialize a new compute buffer
	 *
	 * @param[in]  id       Identifier for this buffer
	 */
	compute_buffer(const std::string &id);

	/**
	 * @brief     Initialize the contents of the buffer for rendering. This method
	 *            must be implemented by derived classes as part of their initialization
	 *            routine.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 */
	virtual void init(const render_context &context);

	/**
	 * @brief     Allocate size-dependent resources for the contents of this buffer.
	 *            This method must be implemented by derived classes to respond to
	 *            rendering size changes.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 */
	virtual void allocate_textures(const render_context &context);

	/**
	 * @brief     Render the contents of this buffer. This method must
	 *            be implemented by derived classes as part of their rendering routine.
	 *            The target framebuffer and renderbuffer are already bound when this
	 *            function is called.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 */
	virtual void dispatch(const render_context &context);

	/**
	 * @brief      Get a reference to the program represented by this buffer
	 *
	 * @return     OpenGL program for this buffer.
	 */
	inline const backends::gx::program &program() const { return host_.program(); }

	/**
	 * @brief      Get a reference to the input array for this buffer
	 *
	 * @return     Reference to the input array for this buffer
	 */
	inline const std::deque<program_input> &inputs() const { return host_.inputs(); }

	/**
	 * @brief      Get a reference to the input array for this buffer
	 *
	 * @return     Reference to the input array for this buffer
	 */
	inline std::deque<program_input> &inputs() { return host_.inputs(); }

	/**
	 * @brief       Get a reference to the current source for this buffer
	 *
	 * @return      Pointer to the source
	 */
	inline const std::unique_ptr<sources::basic_source> &source() const { return host_.source(); }

	/**
	 * @brief            Set the sources for this buffer
	 *
	 * @param new_source New source object to use for compiling this buffer
	 */
	inline void source(std::unique_ptr<sources::basic_source> new_source) { host_.source(std::move(new_source)); }

	/**
	 * @brief Get the program interface for this buffer
	 *
	 * @return Reference to the interface object for this buffer
	 */
	inline const program_interface &interface() const { return host_.program_intf(); }

	/**
	 * @brief Get the number of work groups
	 *
	 * @return Number of work groups in the X, Y and Z dimensions
	 */
	inline glm::uvec3 num_groups() const { return num_groups_; }

	/**
	 * @brief Set the number of work groups
	 *
	 * @param new_groups Number of work groups in the X, Y and Z dimensions
	 */
	inline void num_groups(glm::uvec3 new_groups) { num_groups_ = new_groups; }
};
} // namespace buffers
} // namespace shadertoy

#endif /* SHADERTOY_HAS_COMPUTE */
#endif /* _SHADERTOY_BUFFERS_COMPUTE_BUFFER_HPP_ */

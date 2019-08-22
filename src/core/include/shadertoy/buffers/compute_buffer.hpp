#ifndef _SHADERTOY_BUFFERS_COMPUTE_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_COMPUTE_BUFFER_HPP_

#include "shadertoy/pre.hpp"

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
	 * @brief      Get the current override program template for this buffer
	 *
	 * @return     Pointer to the override program, or null if this buffer is using the default program template
	 */
	inline const std::shared_ptr<compiler::program_template> &override_program() const
	{
		return host_.override_program();
	}

	/**
	 * @brief      Set the current override program template for this buffer
	 *
	 * @param new_program Pointer to the override program template, or null if
	 *                    this buffer should use the default program template
	 */
	inline void override_program(std::shared_ptr<compiler::program_template> new_program)
	{
		host_.override_program(new_program);
	}

	/**
	 * @brief       Get a reference to the current source part for this buffer
	 *
	 * @return      Pointer to the source part
	 */
	inline const std::unique_ptr<compiler::basic_part> &source() const { return host_.source(); }

	/**
	 * @brief       Set the sources for this buffer to the given part
	 *
	 * @param new_part New part to use as sources for this buffer
	 */
	inline void source(std::unique_ptr<compiler::basic_part> new_part)
	{
		host_.source(std::move(new_part));
	}

	/**
	 * @brief       Set the sources for this buffer from the given part
	 *
	 * @param new_source New source string to use for compiling this buffer
	 */
	inline void source(const std::string &new_source) { host_.source(new_source); }

	/**
	 * @brief       Set the sources for this buffer from the given part from a file
	 *
	 * The target file will be read everytime this buffer is being compiled. Use
	 * program_buffer#source if you want to cache the file's contents.
	 *
	 * @param new_file New file to load the sources from
	 */
	inline void source_file(const std::string &new_file) { host_.source_file(new_file); }

	/**
	 * @brief         Get the current source map pointer
	 *
	 * @return        Current source map pointer
	 */
	inline std::map<GLenum, std::string> *source_map() const { return host_.source_map(); }

	/**
	 * @brief         Set the source map pointer for the compiled sources
	 *
	 * The pointer must either be null or remain valid until the sources for this
	 * buffer have been compiled.
	 *
	 * @param new_map New source map pointer
	 */
	inline void source_map(std::map<GLenum, std::string> *new_map) { host_.source_map(new_map); }

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

#endif /* _SHADERTOY_BUFFERS_COMPUTE_BUFFER_HPP_ */

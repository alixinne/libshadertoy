#ifndef _SHADERTOY_BUFFERS_PROGRAM_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_PROGRAM_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/gl_buffer.hpp"

#include "shadertoy/program_host.hpp"

namespace shadertoy
{
namespace buffers
{

/**
 * @brief      Represents a buffer that processes its inputs with a shader
 */
class shadertoy_EXPORT program_buffer : public gl_buffer
{
	/// OpenGL program host
	program_host host_;

#if !SHADERTOY_HAS_PROGRAM_INTERFACE
	/// Output count
	size_t output_count_;
#endif

	protected:
	/**
	 * @brief      Initialize the geometry to use for this buffer
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	virtual void init_geometry(const render_context &context, const io_resource &io) = 0;

	/**
	 * @brief      Render the geometry for this buffer. The inputs, program and framebuffer
	 *             are already bound by the caller.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 */
	virtual void render_geometry(const render_context &context, const io_resource &io) = 0;

	/**
	 * @brief      Render the contents of this buffer.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 * @param[in]  size    Rendering size
	 */
	void render_gl_contents(const render_context &context, const io_resource &io, const rsize &size) override;

	public:
#if SHADERTOY_HAS_PROGRAM_INTERFACE
	/**
	 * @brief      Initialize a new ShaderProgram buffer
	 *
	 * @param[in]  id       Identifier for this buffer
	 */
	program_buffer(const std::string &id);
#else
	/**
	 * @brief      Initialize a new ShaderProgram buffer
	 *
	 * @param[in]  id       Identifier for this buffer
	 * @param[in]  outputs  Number of outputs for this buffer
	 */
	program_buffer(const std::string &id, size_t outputs = 1);
#endif

	/**
	 * @brief      Initialize the contents of this buffer
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void init(const render_context &context, const io_resource &io) override;

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
	 * @brief Obtains the list of outputs for this buffer.
	 *
	 * @return List of discovered program outputs
	 */
	std::optional<std::vector<buffer_output>> get_buffer_outputs() const override;

	/**
	 * @brief Get the program interface for this buffer
	 *
	 * @return Reference to the interface object for this buffer
	 */
	inline const program_interface &interface() const { return host_.program_intf(); }
};
} // namespace buffers
} // namespace shadertoy

#endif /* _SHADERTOY_BUFFERS_PROGRAM_BUFFER_HPP_ */

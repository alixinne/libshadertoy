#ifndef _SHADERTOY_PROGRAM_HOST_HPP_
#define _SHADERTOY_PROGRAM_HOST_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/backends/gx/program.hpp"
#include "shadertoy/program_input.hpp"
#include "shadertoy/program_interface.hpp"
#include "shadertoy/sources/basic_source.hpp"

#include <deque>
#include <map>
#include <memory>

#define SHADERTOY_ICHANNEL_COUNT 4

namespace shadertoy
{

/**
 * @brief Hosts an OpenGL program for a buffer
 */
class shadertoy_EXPORT program_host
{
	/// Buffer program
	std::unique_ptr<backends::gx::program> program_;

	/// Program interface details
	std::unique_ptr<program_interface> program_interface_;

	/// Inputs for this shader
	std::deque<program_input> inputs_;

	/// Source for this buffer should that provides the shader
	std::unique_ptr<sources::basic_source> source_;

	public:
	/**
	 * @brief Initialize a new program host
	 */
	program_host();

	/**
	 * @brief Initialize the hosted program
	 *
	 * This will compile the final program sources and assemble it into
	 * an OpenGL program object.
	 *
	 * @param context Rendering context
	 * @param stage   Target stage. Can be either GL_FRAGMENT_SHADER or GL_COMPUTE_SHADER
	 *
	 * @throws shadertoy_error stage is not a valid target
	 */
	void init_program(const render_context &context, GLenum stage);

	/**
	 * @brief Prepare the program for rendering
	 *
	 * This will use the program, bind its inputs and update the associated
	 * uniforms for an upcoming (caller-issued) draw call.
	 *
	 * @param context Rendering context
	 */
	void prepare_render(const render_context &context);

	/**
	 * @brief      Get a reference to the program represented by this buffer
	 *
	 * @return     OpenGL program for this buffer.
	 */
	inline const backends::gx::program &program() const { return *program_; }

	/**
	 * @brief      Get a reference to the input array for this buffer
	 *
	 * @return     Reference to the input array for this buffer
	 */
	inline const std::deque<program_input> &inputs() const { return inputs_; }

	/**
	 * @brief      Get a reference to the input array for this buffer
	 *
	 * @return     Reference to the input array for this buffer
	 */
	inline std::deque<program_input> &inputs() { return inputs_; }

	/**
	 * @brief       Get a reference to the current source for this buffer
	 *
	 * @return      Pointer to the source
	 */
	inline const std::unique_ptr<sources::basic_source> &source() const { return source_; }

	/**
	 * @brief            Set the sources for this buffer
	 *
	 * @param new_source New source object to use for compiling this buffer
	 */
	inline void source(std::unique_ptr<sources::basic_source> new_source) { source_ = std::move(new_source); }

	/**
	 * @brief         Get the program interface of the hosted program
	 *
	 * The program must have been compiled using init_program.
	 */
	inline const program_interface &program_intf() const { return *program_interface_; }
};
} // namespace shadertoy

#endif /* _SHADERTOY_PROGRAM_HOST_HPP_ */

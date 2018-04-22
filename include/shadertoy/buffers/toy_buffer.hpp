#ifndef _SHADERTOY_BUFFERS_TOY_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_TOY_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/gl_buffer.hpp"

#include "shadertoy/program_input.hpp"

#include <deque>

namespace shadertoy
{
namespace buffers
{

/**
 * @brief      Represents a ShaderToy-like buffer with inputs, programs and output
 */
class shadertoy_EXPORT toy_buffer : public gl_buffer
{
private:
	/// Buffer program
	gl::program program_;

	/// Fragment shader
	gl::shader fs_;

	/// Bound uniform state
	std::vector<std::shared_ptr<shadertoy::bound_inputs_base>> bound_inputs_;

	/// Inputs for this shader
	std::deque<program_input> inputs_;

	/// List of source files
	std::vector<std::string> source_files_;

protected:
	/**
	 * @brief      Initializes the contents of this buffer
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void init_contents(render_context &context, io_resource &io) override;

	/**
	 * @brief      Renders the contents of this buffer.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 */
	void render_gl_contents(render_context &context, io_resource &io) override;

public:
	/**
	 * @brief      Initializes a new ShaderToy buffer
	 *
	 * @param[in]  id       Identifier for this buffer
	 */
	toy_buffer(const std::string &id);

	/**
	 * @brief      Get a reference to the program represented by this buffer
	 *
	 * @return     OpenGL program for this buffer.
	 */
	inline gl::program &program()
	{ return program_; }

	/**
	 * @brief      Get a reference to the input array for this buffer
	 *
	 * @return     Reference to the input array for this buffer
	 */
	inline const std::deque<program_input> &inputs() const
	{ return inputs_; }

	/**
	 * @brief      Get a reference to the input array for this buffer
	 *
	 * @return     Reference to the input array for this buffer
	 */
	inline std::deque<program_input> &inputs()
	{ return inputs_; }

	/**
	 * @brief     Get a reference to the source file list for this buffer
	 *
	 * @return    Reference to the source file for this buffer
	 */
	inline const std::vector<std::string> &source_files() const
	{ return source_files_; }

	/**
	 * @brief     Get a reference to the source file list for this buffer
	 *
	 * @return    Reference to the source file for this buffer
	 */
	inline std::vector<std::string> &source_files()
	{ return source_files_; }
};
}
}

#endif /* _SHADERTOY_BUFFERS_TOY_BUFFER_HPP_ */

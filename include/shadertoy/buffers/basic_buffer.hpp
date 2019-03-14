#ifndef _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/io_resource.hpp"

#include "shadertoy/members/buffer_member.hpp"

#include <optional>
#include <vector>

namespace shadertoy
{
namespace buffers
{

/**
 * @brief Represents a program output location to be used in a buffer
 */
struct buffer_output
{
	/// Name of the buffer output
	std::string name;

	/// Interface location of the output
	GLint location;

	/// Type of the output
	GLenum type;

	/**
	 * @brief Construct a new instance of the buffer_output class
	 *
	 * @param name     Name of the buffer output
	 * @param location Interface location of the output
	 * @param type     Type of the output
	 */
	buffer_output(std::string name, GLint location, GLenum type);
};

/**
 * @brief Represents a buffer in a swap chain.
 *
 * This class only contains shared behavior among all buffers, which mainly
 * includes having textures that are rendered to in a ping-pong manner.
 */
class shadertoy_EXPORT basic_buffer
{
	/// Buffer identifier
	const std::string id_;

	/// Query for iTimeDelta execution time
	gl::query time_delta_query_;

	/// Size of this buffer's output
	rsize_ref render_size_;

protected:
	/**
	 * @brief     Initialize a new base buffer
	 *
	 * @param[in] id        Identifier for this buffer
	 */
	basic_buffer(std::string id);

	/**
	 * @brief     Initialize the contents of the buffer for rendering. This method
	 *            must be implemented by derived classes as part of their initialization
	 *            routine.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	virtual void init_contents(const render_context &context, const io_resource &io) = 0;

	/**
	 * @brief     Allocate size-dependent resources for the contents of this buffer.
	 *            This method must be implemented by derived classes to respond to
	 *            rendering size changes.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	virtual void allocate_contents(const render_context &context, const io_resource &io) = 0;

	/**
	 * @brief     Render the contents of this buffer. This method must
	 *            be implemented by derived classes as part of their rendering routine.
	 *            The target framebuffer and renderbuffer are already bound when this
	 *            function is called.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 * @param[in]  member  Current swap-chain member
	 */
	virtual void render_contents(const render_context &context, const io_resource &io,
								 const members::buffer_member &member) = 0;

public:
	/**
	 * @brief      Obtain the identifier of this buffer
	 *
	 * @return     String identifying this buffer
	 */
	inline const std::string &id() const
	{ return id_; }

	/**
	 * @brief      Obtain the query object used to measure this buffer runtime
	 *
	 * @return     Reference to the query object
	 */
	inline const gl::query &time_delta_query() const
	{ return time_delta_query_; }

	/**
	 * @brief      Prepare the current buffer for rendering
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void init(const render_context &context, const io_resource &io);

	/**
	 * @brief      Allocate the textures for this buffer. Note that the current
	 *             contents of previous textures are discarded.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void allocate_textures(const render_context &context, const io_resource &io);

	/**
	 * @brief      Render the buffer using the current OpenGL context
	 *
	 * @param[in]  context Context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 * @param[in]  member  Current swap-chain member
	 */
	void render(const render_context &context, const io_resource &io, const members::buffer_member &member);

	/**
	 * @brief      Obtain the duration of the last rendering of this buffer, in
	 *             nanoseconds. This method may block while waiting for the
	 *             query object to be available.
	 *
	 * @return     Number of nanoseconds elapsed during the rendering of this
	 *             buffer.
	 */
	uint64_t elapsed_time();

	/**
	 * @brief Obtains the list of outputs for this buffer.
	 *
	 * This method is to be implemented by derived classes which are able to
	 * describe their outputs from their internal representations (i.e.
	 * programs using ARB_program_interface_query).
	 *
	 * @return std::nullopt if the query is not supported, otherwise list of
	 * requested outputs
	 */
	virtual std::optional<std::vector<buffer_output>> get_buffer_outputs() const;
};
}
}

#endif /* _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_ */

#ifndef _SHADERTOY_BUFFERS_BUFFER_BASE_HPP_
#define _SHADERTOY_BUFFERS_BUFFER_BASE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"

namespace shadertoy
{
namespace buffers
{

/**
 * @brief Represents a buffer in a swap chain. Rendering is done using a framebuffer.
 *
 * This class instantiates a framebuffer and a renderbuffer which are bound
 * before rendering the contents of this buffer.
 */
class gl_buffer : public basic_buffer
{
	/// Target framebuffer
	gl::framebuffer target_fbo_;

	/// Target renderbuffer
	gl::renderbuffer target_rbo_;

protected:
	/**
	 * @brief     Initialize a new gl_buffer
	 *
	 * @param[in] id        Identifier for this buffer
	 */
	gl_buffer(const std::string &id);

	/**
	 * @brief     Initialize the contents of the buffer for rendering.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void init_contents(const render_context &context, const io_resource &io) override;

	/**
	 * @brief     Initialize the renderbuffer object for the new specified size.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void allocate_contents(const render_context &context, const io_resource &io) override;

	/**
	 * @brief     Render the contents of this buffer. This methods binds the
	 *            framebuffer and renderbuffer to the appropriate texture for
	 *            rendering, and then calls render_gl_contents as defined by
	 *            the derived class.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 */
	void render_contents(const render_context &context, const io_resource &io) final;

	/**
	 * @brief     Render the contents of this buffer to the currently bound
	 *            framebuffer and renderbuffer. This method must be implemented
	 *            by derived classes as part of their rendering routine.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 */
	virtual void render_gl_contents(const render_context &context, const io_resource &io) = 0;

public:
	/**
	 * @brief Obtain this buffer's GL framebuffer object
	 *
	 * @return Reference to the framebuffer object
	 */
	inline const gl::framebuffer &target_fbo() const
	{ return target_fbo_; }

	/**
	 * @brief Obtain this buffer's GL renderbuffer object
	 *
	 * @return Reference to the renderbuffer object
	 */
	inline const gl::renderbuffer &target_rbo() const
	{ return target_rbo_; }
};
}
}

#endif /* _SHADERTOY_BUFFERS_BUFFER_BASE_HPP_ */

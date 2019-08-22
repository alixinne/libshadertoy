#ifndef _SHADERTOY_BUFFERS_BUFFER_BASE_HPP_
#define _SHADERTOY_BUFFERS_BUFFER_BASE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/graphics_buffer.hpp"

#include "shadertoy/backends/gx/framebuffer.hpp"
#include "shadertoy/backends/gx/renderbuffer.hpp"

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
class gl_buffer : public graphics_buffer
{
	/// Target framebuffer
	std::unique_ptr<backends::gx::framebuffer> target_fbo_;

	/// Target renderbuffer
	std::unique_ptr<backends::gx::renderbuffer> target_rbo_;

protected:
	/**
	 * @brief     Initialize a new gl_buffer
	 *
	 * @param[in] id        Identifier for this buffer
	 */
	gl_buffer(const std::string &id);

	/**
	 * @brief     Render the contents of this buffer to the currently bound
	 *            framebuffer and renderbuffer. This method must be implemented
	 *            by derived classes as part of their rendering routine.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 */
	virtual void render_gl_contents(const render_context &context, const io_resource &io) = 0;

	/**
	 * @brief     Binds the given texture to the target framebuffer object
	 *            for rendering. This method may be overridden by derived classes
	 *            in order to control the binding process. The default behavior is
	 *            to bind the first layer of the texture object to the first color attachment.
	 *
	 * @param[in]  target     Target the framebuffer is bound to
	 * @param[in]  target_fbo Target framebuffer bound object
	 * @param[in]  io         IO resource object containing the target textures to bind
	 */
	virtual void attach_framebuffer_outputs(GLenum target,
											const backends::gx::framebuffer &target_fbo,
											const io_resource &io);

public:
	/**
	 * @brief     Initialize the contents of the buffer for rendering.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void init(const render_context &context, const io_resource &io) override;

	/**
	 * @brief     Initialize the renderbuffer object for the new specified size.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void allocate_textures(const render_context &context, const io_resource &io) override;

	/**
	 * @brief     Render the contents of this buffer. This methods binds the
	 *            framebuffer and renderbuffer to the appropriate texture for
	 *            rendering, and then calls render_gl_contents as defined by
	 *            the derived class.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 * @param[in]  member  Current swap-chain member
	 */
	void render(const render_context &context, const io_resource &io,
				const members::buffer_member &member) final;

	/**
	 * @brief Obtain this buffer's GL framebuffer object
	 *
	 * @return Reference to the framebuffer object
	 */
	inline const backends::gx::framebuffer &target_fbo() const
	{ return *target_fbo_; }

	/**
	 * @brief Obtain this buffer's GL renderbuffer object
	 *
	 * @return Reference to the renderbuffer object
	 */
	inline const backends::gx::renderbuffer &target_rbo() const
	{ return *target_rbo_; }
};
}
}

#endif /* _SHADERTOY_BUFFERS_BUFFER_BASE_HPP_ */

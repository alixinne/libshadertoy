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
	 * @brief     Initializes a new gl_buffer
	 *
	 * @param[in] id        Identifier for this buffer
	 */
	gl_buffer(const std::string &id);

	/**
	 * @brief     Initializes the contents of the buffer for rendering.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  width   Width of the rendering textures for this buffer.
	 * @param[in]  height  Height of the rendering textures for this buffer.
	 */
	void init_contents(render_context &context, int width, int height) override;

	/**
	 * @brief     Initializes the renderbuffer object for the new specified size.
	 *
	 * @param[in] width  New rendering width
	 * @param[in] height New rendering height
	 */
	void allocate_contents(int width, int height) override;

	/**
	 * @brief     Renders the contents of this buffer. This methods binds the
	 *            framebuffer and renderbuffer to the appropriate texture for
	 *            rendering, and then calls render_gl_contents as defined by
	 *            the derived class.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 */
	void render_contents(render_context &context) final;

	/**
	 * @brief     Renders the contents of this buffer to the currently bound
	 *            framebuffer and renderbuffer. This method must be implemented
	 *            by derived classes as part of their rendering routine.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 */
	virtual void render_gl_contents(render_context &context) = 0;

public:
	/**
	 * @brief Obtains this buffer's GL framebuffer object
	 *
	 * @return Reference to the framebuffer object
	 */
	inline gl::framebuffer &target_fbo()
	{ return target_fbo_; }

	/**
	 * @brief Obtains this buffer's GL renderbuffer object
	 *
	 * @return Reference to the renderbuffer object
	 */
	inline gl::renderbuffer &target_rbo()
	{ return target_rbo_; }
};
}
}

#endif /* _SHADERTOY_BUFFERS_BUFFER_BASE_HPP_ */

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

	/// Clear color
	std::array<float, 4> clear_color_;

	/// Clear depth
	float clear_depth_;

	/// Clear flags
	GLbitfield clear_bits_;

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

	/**
	 * @brief     Binds the given texture to the target framebuffer object
	 *            for rendering. This method may be overridden by derived classes
	 *            in order to control the binding process. The default behavior is
	 *            to bind the first layer of the texture object to the first color attachment.
	 *
	 * @param[in]  target_fbo Target framebuffer bound object
	 * @param[in]  texture    Target texture object to bind
	 */
	virtual void attach_framebuffer_outputs(const gl::bound_ops<gl::framebuffer> &target_fbo,
											const gl::texture &texture);

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

	/**
	 * @brief Get the current clear color for this buffer
	 *
	 * The default is (0.f, 0.f, 0.f, 0.f)
	 *
	 * @return Clear color RGBA components
	 */
	inline const std::array<float, 4> &clear_color() const
	{ return clear_color_; }

	/**
	 * @brief Sets the clear color for this buffer
	 *
	 * @param new_color New clear color
	 */
	inline void clear_color(const std::array<float, 4> &new_color)
	{ clear_color_ = new_color; }

	/**
	 * @brief Gets the current clear depth for this buffer
	 *
	 * The default is 0.f.
	 *
	 * @return Clear depth value
	 */
	inline float clear_depth() const
	{ return clear_depth_; }

	/**
	 * @brief Sets the new clear depth for this buffer
	 *
	 * @param new_depth New clear depth
	 */
	inline void clear_depth(float new_depth)
	{ clear_depth_ = new_depth; }

	/**
	 * @brief Gets the current clear bits for this buffer
	 *
	 * The default is 0.
	 *
	 * @return Enable clear bits
	 */
	inline GLbitfield clear_bits() const
	{ return clear_bits_; }

	/**
	 * @brief Sets the new clear bits for this buffer
	 *
	 * @param new_bits New clear bits
	 */
	inline void clear_bits(GLbitfield new_bits)
	{ clear_bits_ = new_bits; }
};
}
}

#endif /* _SHADERTOY_BUFFERS_BUFFER_BASE_HPP_ */

#ifndef _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{
namespace buffers
{

/**
 * @brief Represents a buffer in a swap chain.
 *
 * This class only contains shared behavior among all buffers, which
 * mainly includes having a dedicated framebuffer with associated
 * textures that are rendered to in a ping-pong manner.
 */
class basic_buffer
{
	/// Buffer identifier
	const std::string id_;

	/// Target framebuffer
	gl::framebuffer target_fbo_;

	/// Target renderbuffer
	gl::renderbuffer target_rbo_;

	/// Source texture
	std::shared_ptr<gl::texture> source_tex_;

	/// Target texture
	std::shared_ptr<gl::texture> target_tex_;

	/// Query for iTimeDelta execution time
	gl::query time_delta_query_;

protected:
	/**
	 * @brief     Initializes a new base buffer
	 *
	 * @param[in] id        Identifier for this buffer
	 */
	basic_buffer(const std::string &id);

	/**
	 * @brief     Initializes the contents of the buffer for rendering. This method
	 *            must be implemented by derived classes as part of their initialization
	 *            routine.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  width   Width of the rendering textures for this buffer.
	 * @param[in]  height  Height of the rendering textures for this buffer.
	 */
	virtual void init_contents(render_context &context, int width, int height) = 0;

	/**
	 * @brief     Renders the contents of this buffer. This method must
	 *            be implemented by derived classes as part of their rendering routine.
	 *            The target framebuffer and renderbuffer are already bound when this
	 *            function is called.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 */
	virtual void render_contents(render_context &context) = 0;

public:
	/**
	 * @brief      Obtains the identifier of this buffer
	 *
	 * @return     String identifying this buffer
	 */
	inline const std::string &id() const
	{ return id_; }

	/**
	 * @brief      Obtains the query object used to measure this buffer runtime
	 *
	 * @return     Reference to the query object
	 */
	inline gl::query &time_delta_query()
	{ return time_delta_query_; }

	/**
	 * @brief      Prepare the current buffer for rendering
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  width   Width of the rendering textures for this buffer.
	 * @param[in]  height  Height of the rendering textures for this buffer.
	 */
	void init(render_context &context, int width, int height);

	/**
	 * @brief      Allocates the textures for this buffer. Note that the current
	 *             contents of previous textures are discarded.
	 *
	 * @param[in]  width   Width of the texture
	 * @param[in]  height  Height of the texture
	 */
	void allocate_textures(int width, int height);

	/**
	 * @brief      Get a reference to the source texture for this buffer
	 *
	 * @return     Source texture for this buffer.
	 */
	inline std::shared_ptr<gl::texture> source_texture() const
	{ return source_tex_; }

	/**
	 * @brief      Get a reference to the current texture for this buffer
	 *
	 * @return     Target (current) texture for this buffer.
	 */
	inline std::shared_ptr<gl::texture> target_texture() const
	{ return target_tex_; }

	/**
	 * @brief      Render the buffer using the current OpenGL context
	 *
	 * @param[in]  context Context to use for rendering this buffer
	 */
	void render(render_context &context);

	/**
	 * @brief      Obtain the duration of the last rendering of this buffer, in
	 *             nanoseconds. This method may block while waiting for the
	 *             query object to be available.
	 *
	 * @return     Number of nanoseconds elapsed during the rendering of this
	 *             buffer.
	 */
	unsigned long long elapsed_time();

private:
	/**
	 * Initialize a texture that can be used to render this buffer.
	 *
	 * @param tex    Allocated texture object reference
	 * @param width  Width of the target texture
	 * @param height Height of the target texture
	 */
	void init_render_texture(std::shared_ptr<gl::texture> &tex, int width, int height);
};
}
}

#endif /* _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_ */

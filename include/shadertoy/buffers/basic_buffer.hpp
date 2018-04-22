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
 * This class only contains shared behavior among all buffers, which mainly
 * includes having textures that are rendered to in a ping-pong manner.
 */
class shadertoy_EXPORT basic_buffer
{
	/// Buffer identifier
	const std::string id_;

	/// Source texture
	std::shared_ptr<gl::texture> source_tex_;

	/// Target texture
	std::shared_ptr<gl::texture> target_tex_;

	/// Query for iTimeDelta execution time
	gl::query time_delta_query_;

	/// Size of this buffer's output
	rsize_ref render_size_;

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
	 */
	virtual void init_contents(render_context &context) = 0;

	/**
	 * @brief     Allocates size-dependent resources for the contents of this buffer.
	 *            This method must be implemented by derived classes to respond to
	 *            rendering size changes.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 */
	virtual void allocate_contents(render_context &context) = 0;

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
	 */
	void init(render_context &context);

	/**
	 * @brief      Allocates the textures for this buffer. Note that the current
	 *             contents of previous textures are discarded.
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 */
	void allocate_textures(render_context &context);

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

	/**
	 * @brief      Obtains the rendering size object that specifies the size of
	 *             this buffer's output.
	 *
	 * @return     rsize_ref currently in use
	 */
	inline const rsize_ref &render_size() const { return render_size_; }

	/**
	 * @brief      Obtains the rendering size object that specifies the size of
	 *             this buffer's output.
	 *
	 * @return     rsize_ref currently in use
	 */
	inline rsize_ref &render_size() { return render_size_; }

	/**
	 * @brief      Sets the rendering size object that specifies the size of
	 *             this buffer's output. Note that allocate_textures must be
	 *             called after changing this property.
	 *
	 * @param[in]  new_size  New rendering size object
	 */
	inline void render_size(rsize_ref &&new_size) { render_size_ = std::move(new_size); }

private:
	/**
	 * Initialize a texture that can be used to render this buffer.
	 * The size of the texture is defined by the rendering size of this buffer.
	 *
	 * @param context Context to use for texture allocation
	 * @param tex     Allocated texture object reference
	 */
	void init_render_texture(render_context &context, std::shared_ptr<gl::texture> &tex);
};
}
}

#endif /* _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_ */

#ifndef _SHADERTOY_TOY_BUFFER_HPP_
#define _SHADERTOY_TOY_BUFFER_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief      Represents a ShaderToy-like buffer with inputs, programs and output
 */
class shadertoy_EXPORT toy_buffer
{
private:
  /// Render context
  render_context &context_;

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

  /// Buffer program
  gl::program program_;

  /// Fragment shader
  gl::shader fs_;

  /// Bound uniform state
  std::vector<std::shared_ptr<shadertoy::bound_inputs_base>> bound_inputs_;

  /// Query for iTimeDelta execution time
  gl::query time_delta_query_;

public:
  /**
   * @brief      Initializes a new ShaderToy buffer
   *
   * @param      context  Rendering context
   * @param[in]  id       Identifier for this buffer
   */
  toy_buffer(render_context &context, const std::string &id);

  /**
   * @brief      Prepare the current buffer for rendering
   *
   * @param[in]  width   Width of the rendering textures for this buffer.
   * @param[in]  height  Height of the rendering textures for this buffer.
   */
  void init(int width, int height);

  /**
   * @brief      Allocates the textures for this buffer. Note that the current
   *             contents of previous textures are discarded.
   *
   * @param[in]  width   Width of the texture
   * @param[in]  height  Height of the texture
   */
  void allocate_textures(int width, int height);

  /**
   * @brief      Render the buffer using the current OpenGL context
   */
  void render();

  /**
   * @brief      Get a reference to the source texture for this buffer
   *
   * @return     Source texture for this buffer.
   */
  inline std::shared_ptr<gl::texture> source_texture() const {
    return source_tex_; }

	/**
	 * @brief      Get a reference to the current texture for this buffer
	 *
	 * @return     Target (current) texture for this buffer.
	 */
	inline std::shared_ptr<gl::texture> target_texture() const
	{ return target_tex_; }

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

#endif /* _SHADERTOY_TOY_BUFFER_HPP_ */

#ifndef _SHADERTOY_TOY_BUFFER_HPP_
#define _SHADERTOY_TOY_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffer_base.hpp"

namespace shadertoy
{

/**
 * @brief      Represents a ShaderToy-like buffer with inputs, programs and output
 */
class shadertoy_EXPORT toy_buffer : public buffer_base
{
private:
	/// Buffer program
	gl::program program_;

	/// Fragment shader
	gl::shader fs_;

	/// Bound uniform state
	std::vector<std::shared_ptr<shadertoy::bound_inputs_base>> bound_inputs_;

protected:
	/**
	 * @brief      Initializes the contents of this buffer
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  width   Width of the rendering textures for this buffer
	 * @param[in]  height  Height of the rendering textures for this buffer
	 */
	virtual void init_contents(render_context &context, int width, int height) override;

	/**
	 * @brief      Renders the contents of this buffer.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 */
	virtual void render_contents(render_context &context) override;

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
};
}

#endif /* _SHADERTOY_TOY_BUFFER_HPP_ */

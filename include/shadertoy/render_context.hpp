#ifndef _SHADERTOY_RENDER_CONTEXT_HPP_
#define _SHADERTOY_RENDER_CONTEXT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/shader_template.hpp"

namespace shadertoy
{

/**
 * @brief      Represents a context for rendering a specific ShaderToy program.
 */
class shadertoy_EXPORT render_context
{
	/// Program for screen quad
	gl::program screen_prog_;

	/// Vertex shader for screen quad
	gl::shader screen_vs_;

	/// Fragment shader for screen quad
	gl::shader screen_fs_;

	/// Vertex buffer for screen quad
	gl::buffer screen_quad_corners_;
	/// Index buffer for screen quad
	gl::buffer scren_quad_indices_;

	/// Buffer source template
	compiler::shader_template buffer_template_;

	/// Uniform state
	shader_inputs_t state_;

	/// Default error input
	std::shared_ptr<inputs::error_input> error_input_;

	// Callbacks
protected:
	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             providing supplementary sources to add in the current template
	 *             insert in individual buffer fragment shaders.
	 *
	 * @param      buffer_template  Shader template object to add sources to
	 */
	virtual void load_buffer_sources(compiler::shader_template &buffer_template) const;

	/**
	 * @brief      When implemented in a dervied class, provides a callback for
	 *             adding supplementary inputs to be bound to a given program.
	 *
	 * @param      inputs   List which should hold the bound inputs to be
	 *                      applied to the target program.
	 * @param      program  Target shader program.
	 */
	virtual void bind_inputs(std::vector<std::shared_ptr<bound_inputs_base>> &inputs,
							 const gl::program &program) const;

public:
	/**
	 * @brief      Create a new render context.
	 */
	render_context();

	/**
	 * @brief      Get the screen program object to render textures to the screen
	 *
	 * @return     Reference to the screen program object
	 */
	inline const gl::program &screen_prog() const
	{ return screen_prog_; }

	/**
	 * @brief        Initializes the given swap chain
	 *
	 * @param chain  Swap chain to initialize
	 */
	void init(swap_chain &chain) const;

	/**
	 * @brief        Reallocates the textures used by the swap chain \p chain
	 *
	 * @param chain  Swap chain to allocate the textures
	 */
	void allocate_textures(swap_chain &chain) const;

	/**
	 * @brief      Render \p chain using the current context
	 *
	 * @return     Result of \c chain::render
	 */
	std::shared_ptr<members::basic_member> render(swap_chain &chain) const;

	/**
	 * @brief      Compiles a fragment shader for use in a ToyBuffer.
	 *
	 * @param buffer Buffer being compiled
	 * @param fs     Fragment shader object to compile to.
	 */
	void build_buffer_shader(const buffers::toy_buffer &buffer, gl::shader &fs) const;

	/**
	 * @brief      Gets a reference to the uniform state container
	 */
	inline const shader_inputs_t &state() const
	{ return state_; }

	/**
	 * @brief      Gets a reference to the uniform state container
	 */
	inline shader_inputs_t &state()
	{ return state_; }

	/**
	 * @brief     Gets a reference to the buffer template
	 */
	inline const compiler::shader_template &buffer_template() const
	{ return buffer_template_; }

	/**
	 * @brief     Gets a reference to the buffer template
	 */
	inline compiler::shader_template &buffer_template()
	{ return buffer_template_; }

	/**
	 * @brief     Gets a reference to the default error_input instance
	 */
	inline const std::shared_ptr<inputs::error_input> &error_input() const
	{ return error_input_; }

	/**
	 * @brief      Binds uniforms to an actual program, returning the handle object to these bound uniforms.
	 *
	 * @param program Program to bind to
	 * @return
	 */
	std::vector<std::shared_ptr<bound_inputs_base>> bound_inputs(gl::program &program) const;

	/**
	 * @brief      Render a screen quad using the current context
	 */
	void render_screen_quad() const;

	/**
	 * @brief      Render a screen quad using the current context
	 *
	 * @param timerQuery Query object to use for measuring the runtime of the
	 *                   draw call.
	 */
	void render_screen_quad(const gl::query &timerQuery) const;

	/**
	 * @brief      Get the default screen quad vertex shader
	 */
	inline const gl::shader &screen_quad_vertex_shader() const
	{ return screen_vs_; }
};

}

#endif /* _SHADERTOY_RENDER_CONTEXT_HPP_ */

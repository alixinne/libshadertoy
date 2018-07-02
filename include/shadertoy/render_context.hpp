#ifndef _SHADERTOY_RENDER_CONTEXT_HPP_
#define _SHADERTOY_RENDER_CONTEXT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/shader_template.hpp"

namespace shadertoy
{

/**
 * @brief      Represent a context for rendering a specific ShaderToy program.
 *
 * The render context manages shared resources among a set of buffers, which may or may not be
 * members of the same swap chain. This includes the screen drawing programs, the fallback error
 * input, the shared uniforms and the buffer template.
 *
 * The buffer template specifies how the sources of a buffers::program_buffer will be composed with
 * the default sources to make a full fragment shader.
 *
 * The default parts of the buffer template are as follows:
 *   * `glsl:header`: Fragment shader header
 * ```
 * #version 330
 * ```
 *   * `glsl:defines`: List of pre-processor defines
 * ```
 * // Generated on the fly depending on its value
 * // Example:
 * #define MY_VALUE 10
 * ```
 *   * `shadertoy:header`: Header for Shadertoy compatibility
 * ```
 * precision highp float;
 * precision highp int;
 * precision highp sampler2D;
 *
 * // Input texture coordinate
 * in vec2 vtexCoord;
 * // Output fragment color
 * out vec4 fragColor;
 * ```
 *   * `shadertoy:uniforms`: Uniform variables defined by the render context
 * ```
 * // Generated on the fly from the definitions in uniform_state.hpp
 * uniform vec3 iResolution;
 * uniform vec4 iMouse;
 * // etc.
 * ```
 *   * `buffer:inputs`: Sampler uniforms defined by the buffer being compiled
 * ```
 * // Generated on the fly from the input definitions
 * uniform sampler2D myTexture;
 * uniform sampler3D my3DTexture;
 * ```
 *   * `buffer:sources`: Sources provided by the buffer being compiled
 * ```
 * // Should define mainImage, as in a Shadertoy
 * void mainImage(out vec4 O, in vec2 U) { O = vec4(1.); }
 * ```
 *   * `shadertoy:footer`: Footer for Shadertoy compatibility
 * ```
 * // GLSL entry point
 * void main(void) {
 *     fragColor = vec4(0.,0.,0.,1.);
 *     mainImage(fragColor, vtexCoord.xy * iResolution.xy);
 * }
 * ```
 *
 * These parts may be overriden in order to fully control how the resulting shaders are built.
 * Note that the `buffer:*` parts are filled in by render_context#build_buffer_shader from the
 * properties of the current buffer, so they should be present in any template used with the context.
 * Other parts are not mandatory and can be removed.
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
	 * @brief        Initialize the given swap chain
	 *
	 * @param chain  Swap chain to initialize
	 */
	void init(swap_chain &chain) const;

	/**
	 * @brief        Reallocate the textures used by the swap chain \p chain
	 *
	 * @param chain  Swap chain to allocate the textures
	 */
	void allocate_textures(swap_chain &chain) const;

	/**
	 * @brief      Render \p chain using the current context
	 *
	 * @param      chain Chain to render using this context
	 *
	 * @return     Result of chain#render
	 */
	std::shared_ptr<members::basic_member> render(swap_chain &chain) const;

	/**
	 * @brief      Compile a fragment shader for use in a ToyBuffer.
	 *
	 * @param buffer Buffer being compiled
	 * @param fs     Fragment shader object to compile to.
	 */
	void build_buffer_shader(const buffers::program_buffer &buffer, gl::shader &fs) const;

	/**
	 * @brief  Get a reference to the uniform state container
	 *
	 * @return Reference to the state object
	 */
	inline const shader_inputs_t &state() const
	{ return state_; }

	/**
	 * @brief  Get a reference to the uniform state container
	 *
	 * @return Reference to the state object
	 */
	inline shader_inputs_t &state()
	{ return state_; }

	/**
	 * @brief  Get a reference to the buffer template
	 *
	 * @return Reference to the shader_template in use by this context
	 */
	inline const compiler::shader_template &buffer_template() const
	{ return buffer_template_; }

	/**
	 * @brief  Get a reference to the buffer template
	 *
	 * @return Reference to the shader_template in use by this context
	 */
	inline compiler::shader_template &buffer_template()
	{ return buffer_template_; }

	/**
	 * @brief  Get a reference to the default error_input instance
	 *
	 * @return Reference to the error_input fallback object
	 */
	inline const std::shared_ptr<inputs::error_input> &error_input() const
	{ return error_input_; }

	/**
	 * @brief Bind uniforms to an actual program, returning the handle object to these bound uniforms.
	 *
	 * @param program Program to bind to
	 * @return
	 */
	std::vector<std::shared_ptr<bound_inputs_base>> bind_inputs(gl::program &program) const;

	/**
	 * @brief Render a screen quad using the current context
	 */
	void render_screen_quad() const;

	/**
	 * @brief Render a screen quad using the current context
	 *
	 * @param timerQuery Query object to use for measuring the runtime of the
	 *                   draw call.
	 */
	void render_screen_quad(const gl::query &timerQuery) const;

	/**
	 * @brief  Get the default screen quad vertex shader
	 *
	 * @return Reference to the screen quad vertex shader object
	 */
	inline const gl::shader &screen_quad_vertex_shader() const
	{ return screen_vs_; }
};

}

#endif /* _SHADERTOY_RENDER_CONTEXT_HPP_ */

#ifndef _SHADERTOY_RENDER_CONTEXT_HPP_
#define _SHADERTOY_RENDER_CONTEXT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/program_template.hpp"
#include "shadertoy/geometry/screen_quad.hpp"

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
 * The default fragment shader template is as follows:
 * ```
 * #pragma shadertoy part glsl:header begin
 * #version 440
 * #pragma shadertoy part end
 *
 * precision highp float;
 * precision highp int;
 * precision highp sampler2D;
 *
 * in vec2 vtexCoord;
 * out vec4 fragColor;
 *
 * #pragma shadertoy part *:defines
 *
 * // Standard Shadertoy uniform definitions
 * uniform vec3 iResolution;
 * uniform vec4 iMouse;
 * // etc.
 *
 * #pragma shadertoy part buffer:inputs
 * #pragma shadertoy part buffer:sources
 *
 * void main(void) {
 *     fragColor = vec4(0., 0., 0., 1.);
 *     mainImage(fragColor, vtexCoord.xy * iResolution.xy);
 * }
 * ```
 *
 * The default parts of the buffer template are as follows:
 *   * `glsl:header`: Fragment shader header. Defaults to the parts contents but
 *   can be overriden to change the GLSL version in use.
 *   * `*:defines`: List of pre-processor defines registered in render_context#program_buffer().
 * ```
 * // Generated on the fly depending on its value
 * // Example:
 * #define MY_VALUE 10
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
 *
 * These parts may be overriden in order to fully control how the resulting shaders are built.
 * Note that the `buffer:*` parts are filled in by program_buffer#init_contents() from the
 * properties of the current buffer, so they should be present in any template used with the
 * context.
 */
class shadertoy_EXPORT render_context
{
	/// Program for screen quad
	mutable std::unique_ptr<gl::program> screen_prog_;

	/// Screen quad geometry
	mutable std::unique_ptr<geometry::screen_quad> screen_quad_;

	/// Buffer source template
	compiler::program_template buffer_template_;

	/// Default error input
	std::shared_ptr<inputs::error_input> error_input_;

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
	const gl::program &screen_prog() const;

	/**
	 * @brief      Get the screen quad object
	 *
	 * @return     Reference to the screen quad object
	 */
	const geometry::screen_quad &screen_quad() const;

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
	 * @brief  Get a reference to the buffer template
	 *
	 * @return Reference to the shader_template in use by this context
	 */
	inline const compiler::program_template &buffer_template() const
	{ return buffer_template_; }

	/**
	 * @brief  Get a reference to the buffer template
	 *
	 * @return Reference to the shader_template in use by this context
	 */
	inline compiler::program_template &buffer_template()
	{ return buffer_template_; }

	/**
	 * @brief  Get a reference to the default error_input instance
	 *
	 * @return Reference to the error_input fallback object
	 */
	inline const std::shared_ptr<inputs::error_input> &error_input() const
	{ return error_input_; }
};

}

#endif /* _SHADERTOY_RENDER_CONTEXT_HPP_ */

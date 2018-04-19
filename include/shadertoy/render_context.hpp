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
protected:
	/// Last rendered-to texture
	std::weak_ptr<gl::texture> last_texture_;

	/// Program for screen quad
	gl::program screen_prog_;

private:
	/// Config reference
	context_config &config_;

	/// Vertex shader for screen quad
	gl::shader screen_vs_;

	/// Fragment shader for screen quad
	gl::shader screen_fs_;

	/// Vertex buffer for screen quad
	gl::buffer screen_quad_corners_;
	/// Index buffer for screen quad
	gl::buffer scren_quad_indices_;

	/// Screen quad source texture
	/// Note that this texture will only be allocated if needed
	std::shared_ptr<gl::texture> screen_quad_texture_;

	/// Input texture engine
	std::unique_ptr<texture_engine> tex_engine_;

	/// Aux buffers
	std::map<std::string, std::shared_ptr<buffers::basic_buffer>> buffers_;

	/// Cache for define wrapper
	std::string define_wrapper_;

	/// Buffer source template
	compiler::shader_template buffer_template_;

	/// Raw frame count
	int frame_count_;

	/// Uniform state
	shader_inputs_t state_;

	/**
	 * @brief     Ensures the given render size is valid.
	 *
	 * @param[in] size Size to check
	 *
	 * @throws shadertoy_error When the rendering size is invalid
	 */
	void check_render_size(rsize size);

	// Callbacks
protected:
	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             initializing a custom texture engine instead of the default
	 *             one. Note that this field is invoked after all other member
	 *             fields of this class have been initialized, including the
	 *             config reference.
	 *
	 *             The derived class may also invoke this method to allocate a
	 *             texture engine with `buffer` texture type support, and add
	 *             its own handlers on the allocated instance.
	 *
	 * @return     A pointer to a TextureEngine derived instance, which will be
	 *             used by this instance.
	 */
	virtual std::unique_ptr<texture_engine> build_texture_engine();

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             executing code before the ShaderToy buffers are compiled and
	 *             initialized.
	 */
	virtual void pre_init_buffers();

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             executing code after the ShaderToy buffers have been compiled
	 *             and initialized.
	 */
	virtual void post_init_buffers();

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             providing supplementary sources to add in the current template
	 *             insert in individual buffer fragment shaders.
	 *
	 * @param      buffer_template  Shader template object to add sources to
	 */
	virtual void load_buffer_sources(compiler::shader_template &buffer_template);

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             executing code after an auxiliary buffer has been rendered,
	 *             and its input and output textures swapped.
	 *
	 *             The latest rendered result of this buffer will thus be in the
	 *             *source* texture of the buffer.
	 *
	 * @param[in]  name    Name of the current buffer
	 * @param      buffer  Buffer object
	 */
	virtual void post_render_buffer(const std::string &name,
									std::shared_ptr<buffers::basic_buffer> &buffer);

	/**
	 * @brief      When implemented in a dervied class, provides a callback for
	 *             adding supplementary inputs to be bound to a given program.
	 *
	 * @param      inputs   List which should hold the bound inputs to be
	 *                      applied to the target program.
	 * @param      program  Target shader program.
	 */
	virtual void bind_inputs(std::vector<std::shared_ptr<bound_inputs_base>> &inputs,
							 gl::program &program);

public:
	/**
	 * @brief      Create a new render context.
	 *
	 * @param      config  Configuration for this context.
	 */
	render_context(context_config &config);

	/**
	 * @brief      Initialize a new render context
	 */
	void init();

	/**
	 * @brief      Initializes the ShaderToy-like buffers
	 */
	void init_buffers();

	/**
	 * @brief      Allocates textures based on the current config rendering
	 *             size. This can be useful for implementing framebuffer
	 *             resize, but is more lightweight than calling Initialize,
	 *             which recompiles shaders.
	 */
	void allocate_textures();

	/**
	 * @brief      Clears all caches and buffers, call InitializeBuffers for
	 *             reloading.
	 */
	void clear_state();

	/**
	 * @brief      Render the current frame into the current OpenGL context
	 */
	void render();

	/**
	 * @brief      Retrieves the texture ids to allow an operation which uses
	 *             the texIn texture as an input, and writes to the texOut
	 *             texture. The caller is executing the operation.
	 *
	 * @param[in]  texIn   Input texture id for outside operation
	 * @param[in]  texOut  Output texture id for outside operation
	 */
	void read_write_current_frame(GLuint &texIn, GLuint &texOut);

	/**
	 * @brief      Read the currently rendered result.
	 *
	 * @param      texIn  Input texture
	 */
	void read_current_frame(GLuint &texIn);

	/**
	 * @brief      Get a reference to the texture engine of this render context.
	 *
	 * @return     The texture engine.
	 */
	inline texture_engine &tex_engine()
	{ return *tex_engine_; }

	/**
	 * @brief      Compiles a fragment shader for use in a ToyBuffer.
	 *
	 * @param[in]  id  Buffer configuration identifier.
	 * @param      fs  Fragment shader object to compile to.
	 */
	void build_buffer_shader(const std::string &id,
							 gl::shader &fs);

	/**
	 * @brief      Get the define wrapper for fragment shaders.
	 *
	 * @return     Source code of the preprocessor definition wrapper.
	 */
	const GLchar *define_wrapper() const;

	/**
	 * @brief      Gets a reference to the uniform state container
	 */
	inline shader_inputs_t &state()
	{ return state_; }

	/**
	 * @brief      Gets a reference to the configuration objet for this context.
	 */
	inline context_config &config()
	{ return config_; }

	/**
	 * @brief      Binds uniforms to an actual program, returning the handle object to these bound uniforms.
	 *
	 * @param program Program to bind to
	 * @return
	 */
	std::vector<std::shared_ptr<bound_inputs_base>> bound_inputs(gl::program &program);

	/**
	 * @brief      Resets the current context target using the default viewport size
	 *
	 * @param[in]  level       Gray-level of the output
	 */
	void clear(float level);

	/**
	 * @brief      Render a screen quad using the current context
	 */
	void render_screen_quad();

	/**
	 * @brief      Render a screen quad using the current context
	 *
	 * @param timerQuery Query object to use for measuring the runtime of the
	 *                   draw call.
	 */
	void render_screen_quad(gl::query &timerQuery);

	/**
	 * @brief      Binds the texture containing the shadertoy result as well as
	 *             a program which renders this texture to the viewport. Useful
	 *             for rendering to screen by calling
	 *             RenderContext::RenderScreenQuad().
	 */
	void bind_result();

	/**
	 * @brief      Get the default screen quad vertex shader
	 */
	gl::shader &screen_quad_vertex_shader();

	/**
	 * @brief Obtains the buffer object for the given name.
	 *
	 * @param  name [optional] Name of the buffer object to obtain. If empty,
	 *              returns the image buffer.
	 * @return      Pointer to the buffer object, or a null pointer if no such
	 *              buffer exists.
	 */
	std::shared_ptr<buffers::basic_buffer> buffer(const std::string &name = std::string());
};

}

#endif /* _SHADERTOY_RENDER_CONTEXT_HPP_ */

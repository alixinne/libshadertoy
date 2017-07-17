#ifndef _SHADERTOY_TOY_BUFFER_HPP_
#define _SHADERTOY_TOY_BUFFER_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief      Represents a ShaderToy-like buffer with inputs, programs and output
 */
class shadertoy_EXPORT ToyBuffer
{
private:
	/// Stateless OpenGl context
	oglplus::Context gl;

	/// Render context
	RenderContext &context;

	/// Buffer configuration
	BufferConfig config;

	/// Target framebuffer
	oglplus::Framebuffer targetFbo;

	/// Target renderbuffer
	oglplus::Renderbuffer targetRbo;

	/// Source texture
	std::shared_ptr<oglplus::Texture> sourceTex;

	/// Target texture
	std::shared_ptr<oglplus::Texture> targetTex;

	/// Buffer program
	oglplus::Program program;

	/// Fragment shader
	oglplus::FragmentShader fs;

	/// Bound uniform state
	std::vector<std::shared_ptr<shadertoy::BoundInputsBase>> boundInputs;

public:
	/**
	 * @brief      Initializes a new ShaderToy buffer
	 *
	 * @param      context  Rendering context
	 * @param[in]  config   Source configuration for this buffer
	 */
	ToyBuffer(RenderContext &context, const BufferConfig &config);

	/**
	 * @brief      Prepare the current buffer for rendering
	 *
	 * @param[in]  width   Width of the rendering textures for this buffer.
	 * @param[in]  height  Height of the rendering textures for this buffer.
	 */
	void Initialize(int width, int height);

	/**
	 * @brief      Render the buffer using the current OpenGL context
	 */
	void Render();

	/**
	 * @brief      Swaps the textures used for rendering.
	 */
	void SwapBuffers();

	/**
	 * @brief      Get a reference to the source texture for this buffer
	 *
	 * @return     Source texture for this buffer.
	 */
	std::shared_ptr<oglplus::Texture> GetSourceTexture();

	/**
	 * @brief      Get a reference to the current texture for this buffer
	 *
	 * @return     Target (current) texture for this buffer.
	 */
	std::shared_ptr<oglplus::Texture> GetTargetTexture();

	/**
	 * @brief      Gets the current configuration object for this buffer.
	 *
	 * @return     BufferConfig reference.
	 */
	const BufferConfig &GetConfig() const;

	/**
	 * @brief      Get the program for this buffer.
	 *
	 * @return     The shader program associated with this buffer.
	 */
	oglplus::Program &GetProgram();

private:
	void InitializeRenderTexture(std::shared_ptr<oglplus::Texture> &texptr,
								 int width, int height);
};

}

#endif /* _SHADERTOY_TOY_BUFFER_HPP_ */

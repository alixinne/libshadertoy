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
	/// Render context
	RenderContext &context;

	/// Buffer identifier
	const std::string id;

	/// Target framebuffer
	gl::framebuffer targetFbo;

	/// Target renderbuffer
	gl::renderbuffer targetRbo;

	/// Source texture
	std::shared_ptr<gl::texture> sourceTex;

	/// Target texture
	std::shared_ptr<gl::texture> targetTex;

	/// Buffer program
	gl::program program;

	/// Fragment shader
	gl::shader fs;

	/// Bound uniform state
	std::vector<std::shared_ptr<shadertoy::BoundInputsBase>> boundInputs;

	/// Query for iTimeDelta execution time
	gl::query timeDeltaQuery;

public:
	/**
	 * @brief      Initializes a new ShaderToy buffer
	 *
	 * @param      context  Rendering context
	 * @param[in]  id       Identifier for this buffer
	 */
	ToyBuffer(RenderContext &context, const std::string &id);

	/**
	 * @brief      Prepare the current buffer for rendering
	 *
	 * @param[in]  width   Width of the rendering textures for this buffer.
	 * @param[in]  height  Height of the rendering textures for this buffer.
	 */
	void Initialize(int width, int height);

	/**
	 * @brief      Allocates the textures for this buffer. Note that the current
	 *             contents of previous textures are discarded.
	 *
	 * @param[in]  width   Width of the texture
	 * @param[in]  height  Height of the texture
	 */
	void AllocateTextures(int width, int height);

	/**
	 * @brief      Render the buffer using the current OpenGL context
	 */
	void Render();

	/**
	 * @brief      Get a reference to the source texture for this buffer
	 *
	 * @return     Source texture for this buffer.
	 */
	inline std::shared_ptr<gl::texture> GetSourceTexture() const
	{ return sourceTex; }

	/**
	 * @brief      Get a reference to the current texture for this buffer
	 *
	 * @return     Target (current) texture for this buffer.
	 */
	inline std::shared_ptr<gl::texture> GetTargetTexture() const
	{ return targetTex; }

	/**
	 * @brief      Obtain the duration of the last rendering of this buffer, in
	 *             nanoseconds. This method may block while waiting for the
	 *             query object to be available.
	 *
	 * @return     Number of nanoseconds elapsed during the rendering of this
	 *             buffer.
	 */
	unsigned long long GetElapsedTime();

private:
	void InitializeRenderTexture(std::shared_ptr<gl::texture> &tex, int width, int height);
};

}

#endif /* _SHADERTOY_TOY_BUFFER_HPP_ */

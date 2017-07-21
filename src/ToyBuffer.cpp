#include "stdafx.hpp"
#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/UniformState.hpp"
#include "shadertoy/ToyBuffer.hpp"
#include "shadertoy/TextureEngine.hpp"
#include "shadertoy/RenderContext.hpp"

#include "Resources.h"

using namespace std;
using namespace oglplus;
namespace fs = boost::filesystem;

using namespace shadertoy;

ToyBuffer::ToyBuffer(RenderContext &context,
					 const BufferConfig &config)
	: context(context),
	  config(config),
	  boundInputs()
{
}

void ToyBuffer::Initialize(int width, int height)
{
	// Attach the vertex shader for the screen quad
	program.AttachShader(context.GetScreenQuadVertexShader());

	// Load the fragment shader for this buffer
	context.BuildBufferShader(config, fs);

	// Attach shader
	program.AttachShader(fs);

	// Link the program
	program.Link();

	// Use the program
	program.Use();

	// Bind uniform inputs
	boundInputs = context.GetBoundInputs(program);

	// Allocate render textures
	AllocateTextures(width, height);
}

void ToyBuffer::AllocateTextures(int width, int height)
{
	// Initialize buffer textures
	InitializeRenderTexture(sourceTex, width, height);
	InitializeRenderTexture(targetTex, width, height);

	// Setup render buffers
	targetTex->Bind(TextureTarget::_2D);
	targetRbo.Bind(Renderbuffer::Target::Renderbuffer);
	targetRbo.Storage(Renderbuffer::Target::Renderbuffer,
					  PixelDataInternalFormat::DepthComponent, width, height);
}

void ToyBuffer::Render()
{
	// Update renderbuffer to use the correct target texture
	targetTex->Bind(TextureTarget::_2D);
	targetRbo.Bind(Renderbuffer::Target::Renderbuffer);
	targetFbo.Bind(Framebuffer::Target::Draw);

	targetFbo.AttachTexture(Framebuffer::Target::Draw,
							FramebufferAttachment::Color, *targetTex, 0);

	// Prepare the render target
	context.Clear(0.f);

	// Setup program and its uniforms
	program.Use();

	// Override values in bound inputs 0 (ShaderToy inputs)
	auto &resolutions(static_pointer_cast<ShaderInputsType::BoundInputs>(boundInputs[0])
		->State.V<iChannelResolution>());

	// Setup the texture targets
	for (int i = 0; i < 4; ++i)
	{
		Texture::Active(i + 1);
		// Following have side effects, ensure it runs after we selected the new
		// texture unit
		auto &texture = context.GetTextureEngine()
							   .GetInputTexture(config.inputConfig[i]);
		texture.Bind(TextureTarget::_2D);

		resolutions[i][0] = Texture::Width(TextureTarget::_2D);
		resolutions[i][1] = Texture::Height(TextureTarget::_2D);
		resolutions[i][2] = 1.0f;
	}

	// Set all uniforms
	for (auto &inputs : boundInputs)
		inputs->Apply();

	// Render the program
	context.RenderScreenQuad();

	// Swap texture object pointers
	swap(sourceTex, targetTex);
}

void ToyBuffer::InitializeRenderTexture(shared_ptr<Texture> &texptr, int width, int height)
{
	// Only create a texture object if it is necessary
	if (!texptr)
		texptr = make_shared<Texture>();

	// Allocate texture storage according to width/height
	gl.DirectEXT(TextureTarget::_2D, *texptr)
		.MinFilter(TextureMinFilter::Nearest)
		.MagFilter(TextureMagFilter::Nearest)
		.WrapS(TextureWrap::ClampToEdge)
		.WrapT(TextureWrap::ClampToEdge)
		.Image2D(0, PixelDataInternalFormat::RGBA32F,
				 width, height, 0, PixelDataFormat::BGRA,
				 PixelDataType::UnsignedByte, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	float black[4] = {0.f};
	glClearTexImage(GetName(*texptr),
		0,
		GL_BGRA,
		GL_FLOAT,
		black);
}

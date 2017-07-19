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

	// Bind uniform inputs
	boundInputs = context.GetBoundInputs(program);

	// Use the program
	program.Use();

	// Initialize buffer textures
	InitializeRenderTexture(sourceTex, width, height);
	InitializeRenderTexture(targetTex, width, height);

	// Setup render buffers
	targetTex->Bind(TextureTarget::_2D);

	targetRbo.Bind(Renderbuffer::Target::Renderbuffer);
	targetRbo.Storage(Renderbuffer::Target::Renderbuffer,
					  PixelDataInternalFormat::DepthComponent, width, height);

	targetFbo.Bind(Framebuffer::Target::Draw);
	targetFbo.AttachTexture(Framebuffer::Target::Draw,
							FramebufferAttachment::Color, *targetTex, 0);
}

void ToyBuffer::Render()
{
	// Prepare the render target
	targetFbo.Bind(Framebuffer::Target::Draw);
	context.Clear(0.f);

	// Setup program and its uniforms
	program.Use();

	// Override values in bound inputs 0 (ShaderToy inputs)
	auto &resolutions(static_pointer_cast<ShaderInputsType::BoundInputs>(boundInputs[0])
		->State.V<iChannelResolution>());

	// Setup the texture targets
	for (int i = 0; i < 4; ++i)
	{
		auto &texture = context.GetTextureEngine()
							   .GetInputTexture(config.inputConfig[i]);
		Texture::Active(i + 1);
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

	// Update renderbuffer
	targetTex->Bind(TextureTarget::_2D);
	targetFbo.Bind(Framebuffer::Target::Draw);
	targetFbo.AttachTexture(Framebuffer::Target::Draw,
							FramebufferAttachment::Color, *targetTex, 0);
}

void ToyBuffer::InitializeRenderTexture(shared_ptr<Texture> &texptr, int width, int height)
{
	texptr = make_shared<Texture>();
	gl.DirectEXT(TextureTarget::_2D, *texptr)
		.MinFilter(TextureMinFilter::Nearest)
		.MagFilter(TextureMagFilter::Nearest)
		.WrapS(TextureWrap::Repeat)
		.WrapT(TextureWrap::Repeat)
		.Image2D(0, PixelDataInternalFormat::RGBA32F,
				 width, height, 0, PixelDataFormat::BGRA,
				 PixelDataType::UnsignedByte, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	float black[4];
	memset(black, 0, sizeof(black));
	glClearTexImage(GetName(*texptr),
		0,
		GL_BGRA,
		GL_FLOAT,
		black);
}

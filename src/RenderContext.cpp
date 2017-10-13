#include "stdafx.hpp"
#include "Resources.h"
#include "shadertoy/ContextConfig.hpp"
#include "shadertoy/UniformState.hpp"
#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/ToyBuffer.hpp"
#include "shadertoy/TextureEngine.hpp"
#include "shadertoy/ShaderCompiler.hpp"
#include "shadertoy/RenderContext.hpp"

namespace fs = boost::filesystem;
using namespace std;
using namespace oglplus;
using namespace shadertoy;

shared_ptr<TextureEngine> RenderContext::BuildTextureEngine()
{
	auto engine =  make_shared<TextureEngine>(config);

	engine->RegisterHandler("buffer", InputHandler([this]
		(const InputConfig &inputConfig,
		 bool &skipTextureOptions,
		 bool &skipCache,
		 bool &framebufferSized)
	{
		skipTextureOptions = true;
		skipCache = true;
		// No need to reallocate buffer textures, this is handled by the buffer
		// itself
		framebufferSized = false;

		auto &bufferConfigs = config.bufferConfigs;
		auto bufferIt = bufferConfigs.find(inputConfig.source);

		if (bufferIt == bufferConfigs.end())
		{
			BOOST_LOG_TRIVIAL(warning) << "buffer '" << inputConfig.source
									   << "' not found for input " << inputConfig.id;
			return shared_ptr<Texture>();
		}
		else
		{
			if (frameCount == 0)
			{
				BOOST_LOG_TRIVIAL(info) << "binding '" << inputConfig.source
										<< "' to input " << inputConfig.id;
			}

			auto texture = buffers[inputConfig.source]->GetSourceTexture();

			int minFilter = max((int)inputConfig.minFilter, GL_LINEAR),
				magFilter = (int)inputConfig.magFilter;

			gl.DirectEXT(TextureTarget::_2D, *texture)
				.MinFilter(static_cast<TextureMinFilter>(minFilter))
				.MagFilter(static_cast<TextureMagFilter>(magFilter))
				.WrapS(inputConfig.wrap)
				.WrapT(inputConfig.wrap);

			return texture;
		}
	}));

	return engine;
}

void RenderContext::PreInitializeBuffers()
{
}

void RenderContext::PostInitializeBuffers()
{
}

void RenderContext::LoadBufferSources(vector<pair<string, string>> &sources)
{
}

void RenderContext::PostBufferRender(const string &name,
									 shared_ptr<ToyBuffer> &buffer)
{
}

void RenderContext::BindInputs(vector<shared_ptr<BoundInputsBase>> &inputs,
							   Program &program)
{
}

RenderContext::RenderContext(ContextConfig &config)
	: config(config),
	frameCount(0)
{
	textureEngine = BuildTextureEngine();
}

void RenderContext::Initialize()
{
	// Initialize constant uniforms
	state.V<iResolution>() = Vec3f(config.width, config.height, 1.0f);
	state.V<iTimeDelta>() = 1.0f / (float) config.targetFramerate;
	state.V<iFrameRate>() = (float) config.targetFramerate;

	state.V<iChannel0>() = 1;
	state.V<iChannel1>() = 2;
	state.V<iChannel2>() = 3;
	state.V<iChannel3>() = 4;

	state.V<iChannelTime>() = { 0.f, 0.f, 0.f, 0.f };
	state.V<iSampleRate>() = 48000.f;

	// Compile screen quad vertex shader
	screenVs.Source(string(screenQuad_vsh, screenQuad_vsh + screenQuad_vsh_size));
	screenVs.Compile();

	// Compile screen quad fragment shader
	screenFs.Source(string(screenQuad_fsh, screenQuad_fsh + screenQuad_fsh_size));
	screenFs.Compile();

	// Prepare screen quad program
	screenProg.AttachShader(screenVs);
	screenProg.AttachShader(screenFs);

	// Compile screen program
	screenProg.Link();

	// Setup screen textures
	screenProg.Use();
	Uniform<GLint>(screenProg, "screenTexture").Set(0);

	// Initialize the texture engine
	textureEngine->Initialize();

	// Prepare screen quad geometry
	GLfloat coords[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	// Setup coords
	screenQuadCorners.Bind(Buffer::Target::Array);
	Buffer::Data(Buffer::Target::Array, coords);

	// Setup indices
	screenQuadIndices.Bind(Buffer::Target::ElementArray);
	Buffer::Data(Buffer::Target::ElementArray, sizeof(indices) / sizeof(GLuint), indices);

	// Initialize buffers
	InitializeBuffers();
}

void RenderContext::InitializeBuffers()
{
	// Invoke callback
	PreInitializeBuffers();

	// Prepare the define wrapper
	ostringstream oss;
	for (auto define : config.preprocessorDefines)
	{
		oss << "#define " << define.first;
		if (!define.second.empty()) {
			oss << " " << define.second;
		}
		oss << endl;
	}
	defineWrapper = oss.str();

	// Initialize program buffer
	auto bufferConfigs = config.bufferConfigs;
	for (auto it = bufferConfigs.begin(); it != bufferConfigs.end(); ++it)
	{
		auto buf = make_shared<ToyBuffer>(*this, it->second);
		buf->Initialize(config.width, config.height);
		buffers.insert(make_pair(it->first, buf));
	}

	// Setup position and texCoord attributes for shaders
	screenQuadCorners.Bind(Buffer::Target::Array);
	screenQuadIndices.Bind(Buffer::Target::ElementArray);

	vector<Program *> programs{&screenProg};
	vector<const char *> names{"default screen shader"};

	for (auto it = programs.begin(); it != programs.end(); ++it)
	{
		try
		{
			VertexArrayAttrib(**it, "position")
				.Pointer(3, DataType::Float, false, 5 * sizeof(GLfloat), 0).Enable();
		}
		catch (ProgVarError &ex)
		{
			BOOST_LOG_TRIVIAL(warning) << "'position' unused in " <<
									   *(names.begin() + (it - programs.begin()));
		}
		try
		{
			VertexArrayAttrib(**it, "texCoord")
				.Pointer(2, DataType::Float, false, 5 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat))).Enable();
		}
		catch (ProgVarError &ex)
		{
			BOOST_LOG_TRIVIAL(warning) << "'texCoord' unused in " <<
									   *(names.begin() + (it - programs.begin()));
		}
	}

	lastTexture = weak_ptr<Texture>();

	// Invoke callback
	PostInitializeBuffers();
}

void RenderContext::AllocateTextures()
{
	// Drop the reference to screenQuadTexture, it will be recreated if needed
	screenQuadTexture = shared_ptr<Texture>();

	// Reallocate buffer textures
	for (auto &pair : buffers)
		pair.second->AllocateTextures(config.width, config.height);

	// Reallocate inputs
	textureEngine->ClearState(true);

	// Update the iResolution uniform, as this method can be called after a
	// framebuffer size change
	state.V<iResolution>() = Vec3f(config.width, config.height, 1.0f);
}

void RenderContext::ClearState()
{
	// Clear previous input textures
	textureEngine->ClearState();
	// Clear previous buffers
	buffers.clear();
	// Clear the source cache
	sourceCache.clear();
}

void RenderContext::Render()
{
	for (auto pair : buffers)
	{
		pair.second->Render();
		lastTexture = pair.second->GetSourceTexture();

		PostBufferRender(pair.first, pair.second);
	}

	frameCount++;
}

void RenderContext::DoReadWriteCurrentFrame(GLint &texIn, GLint &texOut)
{
	if (auto currentTex = lastTexture.lock())
	{
		// Allocate the target screen quad texture as it is requested
		if (!screenQuadTexture)
		{
			// Create texture object
			screenQuadTexture = make_shared<Texture>();

			// Setup screenQuadTexture
			gl.DirectEXT(TextureTarget::_2D, *screenQuadTexture)
				.MinFilter(TextureMinFilter::Nearest)
				.MagFilter(TextureMagFilter::Nearest)
				.WrapS(TextureWrap::Repeat)
				.WrapT(TextureWrap::Repeat)
				.Image2D(0, PixelDataInternalFormat::RGBA32F,
						 config.width, config.height, 0, PixelDataFormat::BGRA,
						 PixelDataType::Float, nullptr);
		}

		texIn = GetName(*currentTex);
		texOut = GetName(*screenQuadTexture);

		lastTexture = screenQuadTexture;
	}
	else
	{
		throw runtime_error("DoReadWriteCurrentFrame: lastTexture pointer has expired!");
	}
}

void RenderContext::DoReadCurrentFrame(GLint &texIn)
{
	if (auto currentTex = lastTexture.lock())
	{
		texIn = GetName(*currentTex);
	}
	else
	{
		throw runtime_error("DoReadCurrentFrame: lastTexture pointer has expired!");
	}
}

void RenderContext::BuildBufferShader(const BufferConfig &bufferConfig,
									  FragmentShader &fs)
{
	// Load all source parts
	ShaderCompiler compiler;
	auto &sources(compiler.Sources());

	// Load callback sources
	LoadBufferSources(sources);

	// Add define wrapper
	sources.insert(sources.begin(),
		make_pair(string("generated:define-wrapper"), defineWrapper));

	// Add sources
	for (auto &shaderFile : bufferConfig.shaderFiles)
	{
		sources.push_back(make_pair(shaderFile.string(), LoadShaderSource(shaderFile)));
	}

	// Add default wrapper around code
	sources.insert(sources.begin(), make_pair(
		string("internal:wrapper-header"),
		string(wrapper_header_fsh, wrapper_header_fsh + wrapper_header_fsh_size)));

	// Add source from uniform declarations
	sources.insert(sources.begin() + 1, make_pair(
		string("generated:shader-inputs"),
		ShaderInputsType::GetDefinitions()));

	// Add footer
	sources.push_back(make_pair(
		string("internal:wrapper-footer"),
		string(wrapper_footer_fsh, wrapper_footer_fsh + wrapper_footer_fsh_size)));

	// Load sources into fragment shader and compile
	compiler.Compile(fs);
}

const GLchar *RenderContext::LoadShaderSource(const fs::path &path) throw(runtime_error)
{
	fs::path shaderPath(fs::canonical(path));
	ifstream src(shaderPath.string().c_str());
	string loadedSource;
	loadedSource.assign(istreambuf_iterator<char>(src), istreambuf_iterator<char>());
	if (loadedSource.back() != '\n') {
		loadedSource += "\n";
	}
	sourceCache.insert(make_pair(shaderPath.string(), loadedSource));

	BOOST_LOG_TRIVIAL(info) << "loaded " << shaderPath;

	return sourceCache.find(shaderPath.string())->second.c_str();
}

const GLchar *RenderContext::GetDefineWrapper() const
{
	return defineWrapper.c_str();
}

vector<shared_ptr<BoundInputsBase>> RenderContext::GetBoundInputs(oglplus::Program &program)
{
	vector<shared_ptr<BoundInputsBase>> result;

	// External inputs
	BindInputs(result, program);

	// Default inputs
	result.insert(result.begin(), state.GetBoundInputs(program));

	return result;
}

void RenderContext::Clear(float level)
{
	gl.Viewport(0, 0, config.width, config.height);
	gl.ClearColor(level, level, level, level);
	gl.Clear().ColorBuffer().DepthBuffer();
}

void RenderContext::RenderScreenQuad()
{
	screenQuadCorners.Bind(Buffer::Target::Array);
	gl.DrawElements(PrimitiveType::Triangles, 3 * 2, DataType::UnsignedInt);
}

void RenderContext::BindResult()
{
	// Prepare prog and texture
	screenProg.Use();

	Texture::Active(0);
	Texture::Bind(TextureTarget::_2D, *lastTexture.lock());
}

VertexShader &RenderContext::GetScreenQuadVertexShader()
{
	return screenVs;
}

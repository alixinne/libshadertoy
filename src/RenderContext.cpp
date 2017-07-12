#include "stdafx.hpp"
#include "Resources.h"
#include "shadertoy/ContextConfig.hpp"
#include "shadertoy/UniformState.hpp"
#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/ToyBuffer.hpp"
#include "shadertoy/RenderContext.hpp"

namespace fs = boost::filesystem;
using namespace std;
using namespace oglplus;
using namespace shadertoy;

void RenderContext::PreInitializeBuffers()
{
}

void RenderContext::PostInitializeBuffers()
{
}

void RenderContext::LoadBufferSources(vector<string> &sources)
{
}

void RenderContext::PostAuxBufferRender(const string &name,
										shared_ptr<ToyBuffer> &buffer)
{
}

void RenderContext::BindInputs(vector<shared_ptr<BoundInputsBase>> &inputs,
							   ProgramName &program)
{
}

RenderContext::RenderContext(ContextConfig &config)
	: buffer(),
	config(config),
	auxBuffers(),
	inputTextures(),
	emptyTexture(),
	frameCount(0)
{
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

	// Setup screenQuadTexture
	gl.DirectEXT(TextureTarget::_2D, screenQuadTexture)
		.MinFilter(TextureMinFilter::Nearest)
		.MagFilter(TextureMagFilter::Nearest)
		.WrapS(TextureWrap::Repeat)
		.WrapT(TextureWrap::Repeat)
		.Image2D(0, PixelDataInternalFormat::RGBA32F,
				 config.width, config.height, 0, PixelDataFormat::BGRA,
				 PixelDataType::Float, nullptr);

	// Prepare the empty texture, a nice magenta checkerboard
	emptyTexture = make_shared<Texture>();
	gl.DirectEXT(TextureTarget::_2D, *emptyTexture)
		.MinFilter(TextureMinFilter::Nearest)
		.MagFilter(TextureMagFilter::Nearest)
		.WrapS(TextureWrap::Repeat)
		.WrapT(TextureWrap::Repeat)
		.SwizzleB(TextureSwizzle::Red)
		.Image2D(images::CheckerRedBlack(config.width, config.height, 10, 10));

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
		if (it->first == "image")
		{
			buffer = make_shared<ToyBuffer>(*this, it->second);
			buffer->Initialize(config.width, config.height);
		}
		else
		{
			auto buf = make_shared<ToyBuffer>(*this, it->second);
			buf->Initialize(config.width, config.height);
			auxBuffers.insert(make_pair(it->first, buf));
		}
	}

	// Setup position and texCoord attributes for shaders
	screenQuadCorners.Bind(Buffer::Target::Array);
	screenQuadIndices.Bind(Buffer::Target::ElementArray);

	vector<Program *> programs{&screenProg};
	vector<const char *> names{"default screen shader"};

	programs.push_back(&buffer->GetProgram());
	names.push_back(buffer->GetConfig().name.c_str());

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

	lastTexture = nullptr;

	// Invoke callback
	PostInitializeBuffers();
}

void RenderContext::ClearState()
{
	// Clear previous input textures
	inputTextures.clear();
	// Clear previous buffers
	auxBuffers.clear();
	// Clear previous image buffer
	buffer = shared_ptr<ToyBuffer>();
	// Clear the source cache
	sourceCache.clear();
}

void RenderContext::Render()
{
	for (auto pair : auxBuffers)
	{
		pair.second->Render();
		pair.second->SwapBuffers();

		PostAuxBufferRender(pair.first, pair.second);
	}

	buffer->Render();

	frameCount++;
	lastTexture = buffer->GetTargetTexture().get();
}

void RenderContext::DoReadWriteCurrentFrame(GLint &texIn, GLint &texOut)
{
	Texture *currentTex = lastTexture;
	lastTexture = &screenQuadTexture;
	texIn = GetName(*currentTex);
	texOut = GetName(*lastTexture);
}

void RenderContext::DoReadCurrentFrame(GLint &texIn)
{
	texIn = GetName(*lastTexture);
}

void RenderContext::PostRender()
{
	// Swap rendering buffers
	buffer->SwapBuffers();
}

// Error message from SOIL
extern char *result_string_pointer;

Texture &RenderContext::GetInputTexture(const InputConfig &inputConfig)
{
	if (!inputConfig.enabled())
	{
		return *emptyTexture;
	}

	auto it = inputTextures.find(inputConfig.id);
	if (it == inputTextures.end())
	{
		if (inputConfig.type == "texture")
		{
			auto texture = make_shared<Texture>();
			GLuint texid = SOIL_load_OGL_texture(inputConfig.source.c_str(),
												 SOIL_LOAD_AUTO,
												 GetName(*texture),
												 SOIL_FLAG_INVERT_Y);

			if (texid == 0)
			{
				BOOST_LOG_TRIVIAL(warning) << "failed to load '"
										   << inputConfig.source << "' for input " << inputConfig.id
										   << ": " << result_string_pointer;
				inputTextures.insert(make_pair(inputConfig.id, emptyTexture));
			}
			else
			{
				ApplyTextureOptions(inputConfig, texture);

				BOOST_LOG_TRIVIAL(info) << "loaded '" << inputConfig.source
										<< "' for input " << inputConfig.id;
				inputTextures.insert(make_pair(inputConfig.id, texture));
			}
		}
		else if (inputConfig.type == "buffer")
		{
			auto &bufferConfigs = config.bufferConfigs;
			auto bufferIt = bufferConfigs.find(inputConfig.source);

			if (bufferIt == bufferConfigs.end())
			{
				BOOST_LOG_TRIVIAL(warning) << "buffer '" << inputConfig.source
										   << "' not found for input " << inputConfig.id;
				inputTextures.insert(make_pair(inputConfig.id, emptyTexture));
			}
			else
			{
				if (frameCount == 0)
				{
					BOOST_LOG_TRIVIAL(info) << "binding '" << inputConfig.source
											<< "' to input " << inputConfig.id;
					BOOST_LOG_TRIVIAL(warning)
						<< "wrapping and filtering options not yet supported, defaulted to wrap/nearest";
				}

				return *auxBuffers[inputConfig.source]->GetSourceTexture();
			}
		}
		else if (inputConfig.type == "noise")
		{
			// A noise texture
			auto noiseTexture = make_shared<Texture>();
			gl.DirectEXT(TextureTarget::_2D, *noiseTexture)
				.SwizzleB(TextureSwizzle::Red)
				.SwizzleG(TextureSwizzle::Red)
				.Image2D(images::RandomRedUByte(config.width, config.height));
			ApplyTextureOptions(inputConfig, noiseTexture);

			BOOST_LOG_TRIVIAL(warning) << "generated noise texture for input "
									   << inputConfig.id;
			inputTextures.insert(make_pair(inputConfig.id, noiseTexture));
		}
		else if (inputConfig.type == "checker")
		{
			stringstream ss(inputConfig.source);
			int size = 0;
			ss >> size;
			if (ss.fail()) size = 10;

			// A checkerboard texture
			auto checkerTexture = make_shared<Texture>();
			gl.DirectEXT(TextureTarget::_2D, *checkerTexture)
				.SwizzleB(TextureSwizzle::Red)
				.SwizzleG(TextureSwizzle::Red)
				.Image2D(images::CheckerRedBlack(config.width, config.height,
												 config.width / size, config.height / size));
			ApplyTextureOptions(inputConfig, checkerTexture);

			BOOST_LOG_TRIVIAL(warning) << "generated " << size << "x" << size
									   <<" checker texture for input "
									   << inputConfig.id;
			inputTextures.insert(make_pair(inputConfig.id, checkerTexture));
		}
	}

	return *inputTextures.find(inputConfig.id)->second;
}

void RenderContext::BuildBufferShader(const BufferConfig &bufferConfig,
									  FragmentShader &fs)
{
	// Load all source parts
	vector<string> sources;

	// Load callback sources
	LoadBufferSources(sources);

	// Add define wrapper
	sources.insert(sources.begin(), defineWrapper);

	// Add sources
	for (auto &shaderFile : bufferConfig.shaderFiles)
	{
		sources.push_back(LoadShaderSource(shaderFile));
	}

	// Add default wrapper around code
	sources.insert(sources.begin(), string(wrapper_header_fsh,
										   wrapper_header_fsh + wrapper_header_fsh_size));
	sources.push_back(string(wrapper_footer_fsh,
							 wrapper_footer_fsh + wrapper_footer_fsh_size));

	// Load sources into fragment shader and compile
	vector<const GLchar*> csrc(sources.size());
	transform(sources.begin(), sources.end(),
			  csrc.begin(), [] (const string &src) {
		return src.c_str();
	});
	fs.Source(GLSLStrings(csrc));
	fs.Compile();
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

vector<shared_ptr<BoundInputsBase>> RenderContext::GetBoundInputs(oglplus::ProgramName program)
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

VertexShader &RenderContext::GetScreenQuadVertexShader()
{
	return screenVs;
}

void RenderContext::ApplyTextureOptions(const InputConfig &inputConfig, std::shared_ptr<Texture> &texture)
{
	auto minFilter = inputConfig.minFilter;

	gl.DirectEXT(TextureTarget::_2D, *texture)
		.MinFilter(minFilter)
		.MagFilter(inputConfig.magFilter)
		.WrapS(inputConfig.wrap)
		.WrapT(inputConfig.wrap);

	if ((int)minFilter > GL_LINEAR)
	{
		gl.DirectEXT(TextureTarget::_2D, *texture)
			.GenerateMipmap();
	}
}

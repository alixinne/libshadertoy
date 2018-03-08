#include <fstream>
#include <map>
#include <memory>
#include <sstream>

#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>

#include <boost/variant.hpp>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/gl.hpp"

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
using namespace shadertoy;
using shadertoy::gl::gl_call;

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
			return shared_ptr<gl::texture>();
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

			texture->parameter(GL_TEXTURE_MAG_FILTER, magFilter);
			texture->parameter(GL_TEXTURE_MIN_FILTER, minFilter);
			texture->parameter(GL_TEXTURE_WRAP_S, inputConfig.wrap);
			texture->parameter(GL_TEXTURE_WRAP_T, inputConfig.wrap);

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
							   gl::program &program)
{
}

RenderContext::RenderContext(ContextConfig &config)
	: config(config),
	screenVs(GL_VERTEX_SHADER),
	screenFs(GL_FRAGMENT_SHADER),
	frameCount(0)
{
	textureEngine = BuildTextureEngine();
}

void RenderContext::Initialize()
{
	// Initialize constant uniforms
	state.V<iResolution>() = glm::vec3(config.width, config.height, 1.0f);
	// Note that this will be overriden once query measurements are available
	state.V<iTimeDelta>() = 1.0f / (float) config.targetFramerate;
	state.V<iFrameRate>() = (float) config.targetFramerate;

	state.V<iChannel0>() = 1;
	state.V<iChannel1>() = 2;
	state.V<iChannel2>() = 3;
	state.V<iChannel3>() = 4;

	state.V<iChannelTime>() = { 0.f, 0.f, 0.f, 0.f };
	state.V<iSampleRate>() = 48000.f;

	// Compile screen quad vertex shader
	screenVs.source(string(screenQuad_vsh, screenQuad_vsh + screenQuad_vsh_size));
	screenVs.compile();

	// Compile screen quad fragment shader
	screenFs.source(string(screenQuad_fsh, screenQuad_fsh + screenQuad_fsh_size));
	screenFs.compile();

	// Prepare screen quad program
	screenProg.attach_shader(screenVs);
	screenProg.attach_shader(screenFs);

	// Compile screen program
	screenProg.link();

	// Setup screen textures
	screenProg.use();
	screenProg.get_uniform_location("screenTexture").set_value(0);

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
	screenQuadCorners.data(sizeof(coords),
		static_cast<const GLvoid*>(&coords[0]), GL_STATIC_DRAW);

	// Setup indices
	screenQuadIndices.data(sizeof(indices),
		static_cast<const GLvoid*>(&indices[0]), GL_STATIC_DRAW);

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
		auto buf = make_shared<ToyBuffer>(*this, it->first);
		buf->Initialize(config.width, config.height);
		buffers.insert(make_pair(it->first, buf));
	}

	// Setup position and texCoord attributes for shaders
	screenQuadCorners.bind(GL_ARRAY_BUFFER);
	screenQuadIndices.bind(GL_ELEMENT_ARRAY_BUFFER);

	vector<gl::program *> programs{&screenProg};

	GLint location;
	for (auto it = programs.begin(); it != programs.end(); ++it)
	{
		// bind input "position" to vertex locations (3 floats)
		location = gl_call(glGetAttribLocation, GLuint(**it), "position");
		gl_call(glVertexAttribPointer, location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
		gl_call(glEnableVertexAttribArray, location);

		// bind input "texCoord" to vertex texture coordinates (2 floats)
		location = gl_call(glGetAttribLocation, GLuint(**it), "texCoord");
		gl_call(glVertexAttribPointer, location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		gl_call(glEnableVertexAttribArray, location);
	}

	lastTexture = weak_ptr<gl::texture>();

	// Invoke callback
	PostInitializeBuffers();
}

void RenderContext::AllocateTextures()
{
	// Drop the reference to screenQuadTexture, it will be recreated if needed
	screenQuadTexture = shared_ptr<gl::texture>();

	// Reallocate buffer textures
	for (auto &pair : buffers)
		pair.second->AllocateTextures(config.width, config.height);

	// Reallocate inputs
	textureEngine->ClearState(true);

	// Update the iResolution uniform, as this method can be called after a
	// framebuffer size change
	state.V<iResolution>() = glm::vec3(config.width, config.height, 1.0f);
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

void RenderContext::DoReadWriteCurrentFrame(GLuint &texIn, GLuint &texOut)
{
	if (auto currentTex = lastTexture.lock())
	{
		// Allocate the target screen quad texture as it is requested
		if (!screenQuadTexture)
		{
			// Create texture object
			screenQuadTexture = make_shared<gl::texture>(GL_TEXTURE_2D);

			// Setup screenQuadTexture
			screenQuadTexture->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			screenQuadTexture->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			screenQuadTexture->parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
			screenQuadTexture->parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
			screenQuadTexture->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F,
				config.width, config.height, 0, GL_BGRA, GL_FLOAT, nullptr);
		}

		texIn = *currentTex;
		texOut = *screenQuadTexture;

		lastTexture = screenQuadTexture;
	}
	else
	{
		throw runtime_error("DoReadWriteCurrentFrame: lastTexture pointer has expired!");
	}
}

void RenderContext::DoReadCurrentFrame(GLuint &texIn)
{
	if (auto currentTex = lastTexture.lock())
	{
		texIn = *currentTex;
	}
	else
	{
		throw runtime_error("DoReadCurrentFrame: lastTexture pointer has expired!");
	}
}

void RenderContext::BuildBufferShader(const string &id,
									  gl::shader &fs)
{
	auto &bufferConfig(config.bufferConfigs[id]);

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
		state.GetDefinitions()));

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

vector<shared_ptr<BoundInputsBase>> RenderContext::GetBoundInputs(gl::program &program)
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
	gl_call(glViewport, 0, 0, config.width, config.height);
	gl_call(glClearColor, level, level, level, level);
	gl_call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderContext::RenderScreenQuad()
{
	screenQuadCorners.bind(GL_ARRAY_BUFFER);
	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);
}

void RenderContext::RenderScreenQuad(gl::query &timerQuery)
{
	screenQuadCorners.bind(GL_ARRAY_BUFFER);

	timerQuery.begin(GL_TIME_ELAPSED);

	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);

	timerQuery.end(GL_TIME_ELAPSED);
}

void RenderContext::BindResult()
{
	// Prepare prog and texture
	screenProg.use();

	gl_call(glActiveTexture, GL_TEXTURE0);
	lastTexture.lock()->bind(GL_TEXTURE_2D);
}

gl::shader &RenderContext::GetScreenQuadVertexShader()
{
	return screenVs;
}

shared_ptr<ToyBuffer> RenderContext::GetBufferByName(const string &name)
{
	if (name.empty())
	{
		if (buffers.empty())
		{
			return shared_ptr<ToyBuffer>();
		}

		return buffers.rbegin()->second;
	}
	else
	{
		auto it = buffers.find(name);
		if (it == buffers.end())
		{
			return shared_ptr<ToyBuffer>();
		}

		return it->second;
	}
}

#include <fstream>
#include <map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/variant.hpp>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL.hpp"

#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/ContextConfig.hpp"
#include "shadertoy/UniformState.hpp"
#include "shadertoy/ToyBuffer.hpp"
#include "shadertoy/TextureEngine.hpp"
#include "shadertoy/RenderContext.hpp"

using namespace std;
namespace fs = boost::filesystem;

using namespace shadertoy;
using shadertoy::OpenGL::glCall;

ToyBuffer::ToyBuffer(RenderContext &context,
					 const std::string &id)
	: context(context),
	  id(id),
	  fs(GL_FRAGMENT_SHADER),
	  boundInputs(),
	  timeDeltaQuery(GL_TIME_ELAPSED)
{
}

void ToyBuffer::Initialize(int width, int height)
{
	auto &config(context.GetConfig().bufferConfigs[id]);

	// Attach the vertex shader for the screen quad
	program.AttachShader(context.GetScreenQuadVertexShader());

	// Load the fragment shader for this buffer
	context.BuildBufferShader(id, fs);

	// Attach shader
	program.AttachShader(fs);

	// Link the program
	program.Link();

	// Use the program
	program.Use();

	// Dump the program if requested
	if (context.GetConfig().dumpShaders)
	{
		GLenum err;
		// Get GL program id
		GLuint progId = GLuint(program);
		// Allocate buffer
		GLint len, actLen;
		glCall(glGetProgramiv, progId, GL_PROGRAM_BINARY_LENGTH, &len);

		char *progBinary = new char[len];
		// Get binary
		GLenum format;
		glGetProgramBinary(progId, len, &actLen, &format, progBinary);
		if ((err = glGetError()) != GL_NO_ERROR)
		{
			delete[] progBinary;

			std::stringstream ss;
			ss << "could not get program binary: " << err;
			throw std::runtime_error(ss.str());
		}

		// Store in file
		fs::path name(config.shaderFiles.front());
		name.replace_extension(".dump");
		BOOST_LOG_TRIVIAL(info) << "dumping program binary to " << name;

		ofstream ofs(name.string().c_str(), std::ofstream::binary);

		if (!ofs.is_open())
		{
			delete[] progBinary;

			std::stringstream ss;
			ss << "could not open output file " << name.string();
			throw std::runtime_error(ss.str());
		}

		ofs.write(progBinary, actLen);
		ofs.close();
		delete[] progBinary;
	}

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
	targetTex->Bind(GL_TEXTURE_2D);
	targetRbo.Bind(GL_RENDERBUFFER);
	targetRbo.Storage(GL_DEPTH_COMPONENT, width, height);
}

void ToyBuffer::Render()
{
	auto &config(context.GetConfig().bufferConfigs[id]);

	// Update renderbuffer to use the correct target texture
	targetTex->Bind(GL_TEXTURE_2D);
	targetRbo.Bind(GL_RENDERBUFFER);
	targetFbo.Bind(GL_DRAW_FRAMEBUFFER);

	targetFbo.Texture(GL_COLOR_ATTACHMENT0, *targetTex, 0);

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
		glCall(glActiveTexture, GL_TEXTURE0 + i + 1);
		// Following have side effects, ensure it runs after we selected the new
		// texture unit
		auto &texture = context.GetTextureEngine()
							   .GetInputTexture(config.inputConfig[i]);
		texture.Bind(GL_TEXTURE_2D);

		glCall(glGetTexLevelParameterfv, GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &resolutions[i][0]);
		glCall(glGetTexLevelParameterfv, GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &resolutions[i][1]);
		resolutions[i][2] = 1.0f;
	}

	// Try to set iTimeDelta
	GLint available = 0;
	timeDeltaQuery.GetObjectiv(GL_QUERY_RESULT_AVAILABLE, &available);
	if (available)
	{
		// Result available, set uniform value
		GLuint64 timeDelta;
		timeDeltaQuery.GetObjectui64v(GL_QUERY_RESULT, &timeDelta);
		static_pointer_cast<ShaderInputsType::BoundInputs>(boundInputs[0])
			->State.V<iTimeDelta>() = timeDelta / 1e9;
	}

	// Set all uniforms
	for (auto &inputs : boundInputs)
		inputs->Apply();

	// Render the program
	context.RenderScreenQuad(timeDeltaQuery);

	// Swap texture object pointers
	swap(sourceTex, targetTex);
}

unsigned long long ToyBuffer::GetElapsedTime()
{
	GLint available = 0;

	// Wait for result to be available
	while (!available)
	{
		timeDeltaQuery.GetObjectiv(GL_QUERY_RESULT_AVAILABLE, &available);
	}

	// Fetch result
	GLuint64 result;
	timeDeltaQuery.GetObjectui64v(GL_QUERY_RESULT, &result);

	return result;
}

void ToyBuffer::InitializeRenderTexture(shared_ptr<OpenGL::Texture> &texptr, int width, int height)
{
	// Only create a texture object if it is necessary
	if (!texptr)
		texptr = make_shared<OpenGL::Texture>(GL_TEXTURE_2D);

	// Allocate texture storage according to width/height
	texptr->Parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	texptr->Parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texptr->Parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texptr->Parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	texptr->Image2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	float black[4] = {0.f};
	glCall(glClearTexImage, GLuint(*texptr),
		0,
		GL_BGRA,
		GL_FLOAT,
		black);
}

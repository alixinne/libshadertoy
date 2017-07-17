#include "stdafx.hpp"
#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/ContextConfig.hpp"
#include "shadertoy/TextureEngine.hpp"

using namespace std;
using namespace oglplus;
using namespace shadertoy;

// Error message from SOIL
extern "C" {
extern char *result_string_pointer;
}

shared_ptr<Texture> TextureEngine::SOILTextureHandler(const InputConfig &inputConfig,
													  bool &skipTextureOptions,
													  bool &skipCache)
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
		return shared_ptr<Texture>();
	}

	ApplyTextureOptions(inputConfig, texture);

	BOOST_LOG_TRIVIAL(info) << "loaded '" << inputConfig.source
							<< "' for input " << inputConfig.id;

	return texture;
}

shared_ptr<Texture> TextureEngine::NoiseTextureHandler(const InputConfig &inputConfig,
													   bool &skipTextureOptions,
													   bool &skipCache)
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

	return noiseTexture;
}

shared_ptr<Texture> TextureEngine::CheckerTextureHandler(const InputConfig &inputConfig,
														 bool &skipTextureOptions,
														 bool &skipCache)
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

	return checkerTexture;
}

TextureEngine::TextureEngine(ContextConfig &config)
	: config(config),
	inputTextures(),
	emptyTexture(),
	handlers()
{
}

void TextureEngine::Initialize()
{
	// Prepare the empty texture, a nice magenta checkerboard
	emptyTexture = make_shared<Texture>();
	gl.DirectEXT(TextureTarget::_2D, *emptyTexture)
		.MinFilter(TextureMinFilter::Nearest)
		.MagFilter(TextureMagFilter::Nearest)
		.WrapS(TextureWrap::Repeat)
		.WrapT(TextureWrap::Repeat)
		.SwizzleB(TextureSwizzle::Red)
		.Image2D(images::CheckerRedBlack(config.width, config.height, 10, 10));
}

void TextureEngine::ClearState()
{
	inputTextures.clear();
}

Texture &TextureEngine::GetInputTexture(const InputConfig &inputConfig)
{
	if (!inputConfig.enabled())
	{
		return *emptyTexture;
	}

	auto it = inputTextures.find(inputConfig.id);
	if (it == inputTextures.end())
	{
		auto handlerIt = handlers.find(inputConfig.type);
		if (handlerIt != handlers.end())
		{
			bool skipTextureOptions = false,
				 skipCache = false;
			auto texture = handlerIt->second(inputConfig,
											 skipTextureOptions,
											 skipCache);

			if (texture)
			{
				if (!skipTextureOptions)
					ApplyTextureOptions(inputConfig, texture);

				if (!skipCache)
					inputTextures.insert(make_pair(inputConfig.id, texture));
			}
			else
			{
				if (!skipCache)
					inputTextures.insert(make_pair(inputConfig.id, emptyTexture));
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << "unknown texture type "
				<< inputConfig.type << " for input " << inputConfig.id;
		}
	}

	return *inputTextures.find(inputConfig.id)->second;
}

void TextureEngine::ApplyTextureOptions(const InputConfig &inputConfig, std::shared_ptr<Texture> &texture)
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

void TextureEngine::RegisterHandler(const string &name,
									InputHandler handler)
{
	if (name.empty())
		throw std::runtime_error("Input handler name cannot be empty.");

	BOOST_LOG_TRIVIAL(debug) << "registered " << name << " texture handler";
	handlers.insert(make_pair(name, handler));
}

#include "stdafx.hpp"
#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/ContextConfig.hpp"
#include "shadertoy/TextureEngine.hpp"

using namespace std;
using namespace std::placeholders;
using namespace oglplus;
using namespace shadertoy;

// Error message from SOIL
extern "C" {
extern char *result_string_pointer;
}

shared_ptr<Texture> TextureEngine::SOILTextureHandler(const InputConfig &inputConfig,
													  bool &skipTextureOptions,
													  bool &skipCache,
													  bool &framebufferSized)
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
													   bool &skipCache,
													   bool &framebufferSized)
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

	framebufferSized = true;

	return noiseTexture;
}

shared_ptr<Texture> TextureEngine::CheckerTextureHandler(const InputConfig &inputConfig,
														 bool &skipTextureOptions,
														 bool &skipCache,
														 bool &framebufferSized)
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

	framebufferSized = true;

	return checkerTexture;
}

TextureEngine::TextureEngine(ContextConfig &config)
	: config(config)
{
	RegisterHandler("texture", bind(&TextureEngine::SOILTextureHandler, this, _1, _2, _3, _4));
	RegisterHandler("noise", bind(&TextureEngine::NoiseTextureHandler, this, _1, _2, _3, _4));
	RegisterHandler("checker", bind(&TextureEngine::CheckerTextureHandler, this, _1, _2, _3, _4));
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
		.Image2D(images::CheckerRedBlack(32, 32, 16, 16));
}
void TextureEngine::ClearState(bool framebufferSizeChange)
{
	if (!framebufferSizeChange)
	{
		// Clear all textures
		inputTextures.clear();
	}
	else
	{
		// Clear only framebuffer textures
		vector<string> clearTextures;

		// Find framebuffer textures
		for (auto &pair : inputTextures)
		{
			if (get<1>(pair.second))
			{
				clearTextures.push_back(pair.first);
			}
		}

		// Clear textures
		for (auto &obsoleteId : clearTextures)
		{
			inputTextures.erase(obsoleteId);
		}
	}
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
				 skipCache = false,
				 framebufferSized = false;
			auto texture = handlerIt->second(inputConfig,
											 skipTextureOptions,
											 skipCache,
											 framebufferSized);

			if (texture)
			{
				if (!skipTextureOptions)
					ApplyTextureOptions(inputConfig, texture);

				if (!skipCache)
					inputTextures.insert(make_pair(inputConfig.id,
						make_tuple(texture, framebufferSized)));
			}
			else
			{
				if (!skipCache)
					inputTextures.insert(make_pair(inputConfig.id,
						make_tuple(emptyTexture, false)));
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << "unknown texture type "
				<< inputConfig.type << " for input " << inputConfig.id;
		}
	}

	return *get<0>(inputTextures.find(inputConfig.id)->second);
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

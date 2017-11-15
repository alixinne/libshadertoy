#include "stdafx.hpp"

#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/ContextConfig.hpp"
#include "shadertoy/TextureEngine.hpp"

using namespace std;
using namespace std::placeholders;
using namespace oglplus;
using namespace shadertoy;

namespace fs = boost::filesystem;

// Error message from SOIL
extern "C" {
extern char *result_string_pointer;
}

shared_ptr<Texture> TextureEngine::SOILTextureHandler(const InputConfig &inputConfig,
													  bool &skipTextureOptions,
													  bool &skipCache,
													  bool &framebufferSized)
{
	if (inputConfig.source.empty())
	{
		BOOST_LOG_TRIVIAL(error) << "Missing source path for input "
								 << inputConfig.id;
		return shared_ptr<Texture>();
	}

	fs::path texPath(inputConfig.source);

	if (!fs::exists(texPath))
	{
		BOOST_LOG_TRIVIAL(error) << texPath << " not found for input "
								 << inputConfig.id;
		return shared_ptr<Texture>();
	}

	shared_ptr<Texture> texture;
	string ext(texPath.extension().string());
	boost::algorithm::to_lower(ext);
	if (ext.compare(".jpg") == 0 || ext.compare(".jpeg") == 0)
	{
		// use libjpeg
		FILE *infile;
		string sp(texPath.string());
		if ((infile = fopen(sp.c_str(), "rb")) == NULL)
		{
			BOOST_LOG_TRIVIAL(error) << "could not open " << texPath
									 << " for reading for input "
									 << inputConfig.id;
		}
		else
		{
			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;
			cinfo.err = jpeg_std_error(&jerr);

			jpeg_create_decompress(&cinfo);
			jpeg_stdio_src(&cinfo, infile);

			jpeg_read_header(&cinfo, TRUE);
			jpeg_start_decompress(&cinfo);

			bool decode = true;
			PixelDataFormat fmt = PixelDataFormat::RGB;
			if (cinfo.output_components == 1)
				fmt = PixelDataFormat::Red;
			else if (cinfo.output_components == 4)
				fmt = PixelDataFormat::RGBA;
			else if (cinfo.output_components != 3)
			{
				BOOST_LOG_TRIVIAL(error) << "unsupported component count for JPEG "
										 << texPath << " for input " << inputConfig.id;
				// Don't decode unknown format
				decode = false;
			}

			if (decode)
			{
				int stride = cinfo.output_width * cinfo.output_components;
				JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo,
															   JPOOL_IMAGE,
															   stride,
															   1);
				char *imgbuf = new char[cinfo.output_height * stride];

				while (cinfo.output_scanline < cinfo.output_height)
				{
					JDIMENSION read_now = jpeg_read_scanlines(&cinfo, buffer, 1);
					size_t off = cinfo.output_scanline - read_now;
					if (inputConfig.vflip)
						off = cinfo.output_height - off - read_now;
					memcpy(&imgbuf[off * stride], buffer[0], stride);
				}

				texture = make_shared<Texture>();
				gl.DirectEXT(TextureTarget::_2D, *texture)
					.Image2D(0, PixelDataInternalFormat::RGBA32F,
							 cinfo.output_width, cinfo.output_height, 0, fmt,
							 PixelDataType::UnsignedByte, imgbuf);

				delete[] imgbuf;
			}

			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
		}
	}
	else
	{
		// other, use SOIL
		texture = make_shared<Texture>();
		GLuint texid = SOIL_load_OGL_texture(inputConfig.source.c_str(),
											 SOIL_LOAD_AUTO,
											 GetName(*texture),
											 inputConfig.vflip ? SOIL_FLAG_INVERT_Y : 0);
		if (texid == 0)
		{
			BOOST_LOG_TRIVIAL(warning) << "failed to load '"
									   << inputConfig.source << "' for input " << inputConfig.id
									   << ": " << result_string_pointer;
			texture = shared_ptr<Texture>();
		}
	}

	if (texture)
	{
		BOOST_LOG_TRIVIAL(info) << "loaded '" << inputConfig.source
								<< "' for input " << inputConfig.id;
	}

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
	if (emptyTexture) return;

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
				else
					return *texture;
			}
			else
			{
				BOOST_LOG_TRIVIAL(error) << "failed loading " <<
					inputConfig.type << " texture for input " << inputConfig.id;

				if (!skipCache)
					inputTextures.insert(make_pair(inputConfig.id,
						make_tuple(emptyTexture, false)));
				else
					return *emptyTexture;
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

#include <map>
#include <sstream>
#include <vector>

#if LIBSHADERTOY_OPENEXR
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfRgbaFile.h>
#endif /* LIBSHADERTOY_OPENEXR */

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <SOIL/SOIL.h>
#include <jpeglib.h>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "shadertoy/buffer_config.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/texture_engine.hpp"

using namespace std;
using namespace std::placeholders;
using namespace shadertoy;
using namespace shadertoy::utils;

namespace fs = boost::filesystem;

// Error message from SOIL
extern "C" {
extern char *result_string_pointer;
}

shared_ptr<gl::texture> texture_engine::soil_texture_handler(const input_config &inputConfig,
                                                             bool &skipTextureOptions,
                                                             bool &skipCache,
                                                             bool &framebufferSized)
{
	if (inputConfig.source.empty())
	{
		log::shadertoy()->error("Missing source path for input {}", inputConfig.id);
		return shared_ptr<gl::texture>();
	}

	fs::path texPath(inputConfig.source);

	if (!fs::exists(texPath))
	{
		log::shadertoy()->error("{} not found for input {}", texPath, inputConfig.id);
		return shared_ptr<gl::texture>();
	}

	shared_ptr<gl::texture> texture;
	string ext(texPath.extension().string());
	boost::algorithm::to_lower(ext);
	if (ext.compare(".jpg") == 0 || ext.compare(".jpeg") == 0)
	{
		// use libjpeg
		FILE *infile;
		string sp(texPath.string());
		if ((infile = fopen(sp.c_str(), "rb")) == NULL)
		{
			log::shadertoy()->error("Could not open {} for reading for input {}", texPath, inputConfig.id);
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
			GLenum fmt = GL_RGB;
			if (cinfo.output_components == 1)
				fmt = GL_RED;
			else if (cinfo.output_components == 4)
				fmt = GL_RGBA;
			else if (cinfo.output_components != 3)
			{
				log::shadertoy()->error("Unsupported component count for JPEG {} for input {}", texPath,
											 inputConfig.id);
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

				texture = make_shared<gl::texture>(GL_TEXTURE_2D);
				texture->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, cinfo.output_width,
					cinfo.output_height, 0, fmt, GL_UNSIGNED_BYTE, imgbuf);

				delete[] imgbuf;
			}

			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
		}
	}
#if LIBSHADERTOY_OPENEXR
	else if (ext.compare(".exr") == 0)
	{
		Imf::RgbaInputFile in(inputConfig.source.c_str());

		Imath::Box2i win = in.dataWindow();

		Imath::V2i dim(win.max.x - win.min.x + 1, win.max.y - win.min.y + 1);

		std::vector<Imf::Rgba> pixelBuffer(dim.x * dim.y);

		// Set buffer stride according to reading direction
		if (inputConfig.vflip)
			in.setFrameBuffer(pixelBuffer.data() + (dim.y - 1) * dim.x, 1, -dim.x);
		else
			in.setFrameBuffer(pixelBuffer.data(), 1, dim.x);

		// Read the whole image
		in.readPixels(win.min.y, win.max.y);

		texture = make_shared<gl::texture>(GL_TEXTURE_2D);
		texture->image_2d(GL_TEXTURE_2D, 0, GL_RGBA16F, dim.x, dim.y, 0, GL_RGBA,
						  GL_HALF_FLOAT, pixelBuffer.data());
	}
#endif /* LIBSHADERTOY_OPENEXR */
	else 
	{
		// other, use SOIL
		texture = make_shared<gl::texture>(GL_TEXTURE_2D);
		GLuint texid = SOIL_load_OGL_texture(inputConfig.source.c_str(),
											 SOIL_LOAD_AUTO,
											 GLuint(*texture),
											 inputConfig.vflip ? SOIL_FLAG_INVERT_Y : 0);
		if (texid == 0)
		{
			log::shadertoy()->error("Failed to load '{}' for input {}: {}", inputConfig.source, inputConfig.id,
										 result_string_pointer);
			texture = shared_ptr<gl::texture>();
		}
	}

	if (texture)
	{
		log::shadertoy()->info("Loaded '{}' for input {}", inputConfig.source, inputConfig.id);
	}

	return texture;
}

shared_ptr<gl::texture> texture_engine::noise_texture_handler(const input_config &inputConfig,
															  bool &skipTextureOptions,
															  bool &skipCache,
															  bool &framebufferSized)
{
	// A noise texture
	auto noiseTexture = make_shared<gl::texture>(GL_TEXTURE_2D);
	noiseTexture->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	noiseTexture->parameter(GL_TEXTURE_SWIZZLE_G, GL_RED);

	// Create the actual noise
	vector<unsigned char> rnd(config_.width * config_.height);
	generate(rnd.begin(), rnd.end(), []() { return rand() % 256; });

	// Load it
	noiseTexture->image_2d(GL_TEXTURE_2D, 0, GL_RED, config_.width, config_.height,
		0, GL_RED, GL_UNSIGNED_BYTE, rnd.data());

	log::shadertoy()->info("Generated noise texture for input {}", inputConfig.id);

	framebufferSized = true;

	return noiseTexture;
}

shared_ptr<gl::texture> texture_engine::checker_texture_handler(const input_config &inputConfig,
																bool &skipTextureOptions,
																bool &skipCache,
																bool &framebufferSized)
{
	stringstream ss(inputConfig.source);
	int size = 0;
	ss >> size;
	if (ss.fail()) size = 10;

	// A checkerboard texture
	auto checkerTexture = make_shared<gl::texture>(GL_TEXTURE_2D);
	checkerTexture->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	checkerTexture->parameter(GL_TEXTURE_SWIZZLE_G, GL_RED);

	// Generate the checkerboard
	vector<unsigned char> chk(config_.width * config_.height);
	for (int i = 0; i < config_.width; ++i)
		for (int j = 1; j < config_.height; ++j)
			chk[j * config_.height + i] = ((i / size) % 2 == 0) ^ ((j / size) % 2 == 0) ? 255 : 0;

	// Load it
	checkerTexture->image_2d(GL_TEXTURE_2D, 0, GL_RED, config_.width, config_.height,
		0, GL_RED, GL_UNSIGNED_BYTE, chk.data());

	log::shadertoy()->info("Generated {}x{} checker texture for input {}", size, size, inputConfig.id);

	framebufferSized = true;

	return checkerTexture;
}

texture_engine::texture_engine(context_config &config)
	: config_(config)
{
	register_handler("texture", bind(&texture_engine::soil_texture_handler, this, _1, _2, _3, _4));
	register_handler("noise", bind(&texture_engine::noise_texture_handler, this, _1, _2, _3, _4));
	register_handler("checker", bind(&texture_engine::checker_texture_handler, this, _1, _2, _3, _4));
}

void texture_engine::init()
{
	// Prepare the empty texture, a nice magenta checkerboard
	if (empty_texture_) return;

	empty_texture_ = make_shared<gl::texture>(GL_TEXTURE_2D);

	// Generate the checkerboard
	int width = 32, height = 32,
		size = 16;
	vector<unsigned char> chk(width * height);
	for (int i = 0; i < width; ++i)
		for (int j = 1; j < height; ++j)
			chk[j * height + i] = ((i / size) % 2 == 0) ^ ((j / size) % 2 == 0) ? 255 : 0;

	// Load it and set parameters
	empty_texture_->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	empty_texture_->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	empty_texture_->parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	empty_texture_->parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	empty_texture_->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	empty_texture_->image_2d(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
		GL_UNSIGNED_BYTE, chk.data());
}

void texture_engine::clear(bool framebufferSizeChange)
{
	if (!framebufferSizeChange)
	{
		// Clear all textures
		input_textures_.clear();
	}
	else
	{
		// Clear only framebuffer textures
		vector<string> clearTextures;

		// Find framebuffer textures
		for (auto &pair : input_textures_)
		{
			if (get<1>(pair.second))
			{
				clearTextures.push_back(pair.first);
			}
		}

		// Clear textures
		for (auto &obsoleteId : clearTextures)
		{
			input_textures_.erase(obsoleteId);
		}
	}
}

gl::texture &texture_engine::input_texture(const input_config &inputConfig)
{
	if (!inputConfig.enabled())
	{
		return *empty_texture_;
	}

	auto it = input_textures_.find(inputConfig.id);
	if (it == input_textures_.end())
	{
		auto handlerIt = handlers_.find(inputConfig.type);
		if (handlerIt != handlers_.end())
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
					apply_texture_options(inputConfig, *texture);

				if (!skipCache)
					input_textures_.insert(make_pair(inputConfig.id,
						make_tuple(texture, framebufferSized)));
				else
					return *texture;
			}
			else
			{
				log::shadertoy()->error("Failed loading {} texture for input {}", inputConfig.type,
											 inputConfig.id);

				if (!skipCache)
					input_textures_.insert(make_pair(inputConfig.id,
						make_tuple(empty_texture_, false)));
				else
					return *empty_texture_;
			}
		}
		else
		{
			log::shadertoy()->error("Unknown texture type {} for input {}", inputConfig.type, inputConfig.id);
		}
	}

	return *get<0>(input_textures_.find(inputConfig.id)->second);
}

void texture_engine::apply_texture_options(const input_config &inputConfig, gl::texture &texture)
{
	GLint minFilter = inputConfig.min_filter;

	texture.parameter(GL_TEXTURE_MIN_FILTER, minFilter);
	texture.parameter(GL_TEXTURE_MAG_FILTER, inputConfig.mag_filter);
	texture.parameter(GL_TEXTURE_WRAP_S, inputConfig.wrap);
	texture.parameter(GL_TEXTURE_WRAP_T, inputConfig.wrap);

	if (minFilter > GL_LINEAR)
	{
		log::shadertoy()->debug("Generating mipmaps for {}", inputConfig.id);
		texture.generate_mipmap();
	}
}

void texture_engine::register_handler(const string &name,
									  input_handler handler)
{
	if (name.empty())
		throw std::runtime_error("Input handler name cannot be empty.");

	log::shadertoy()->debug("Registered {} texture handler", name);
	handlers_.insert(make_pair(name, handler));
}

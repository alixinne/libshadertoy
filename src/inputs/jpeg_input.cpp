#include <cstdio>

#include <epoxy/gl.h>

#include <jpeglib.h>

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "shadertoy/inputs/jpeg_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;
using namespace shadertoy::utils;

std::shared_ptr<gl::texture> jpeg_input::load_file(const std::string &filename, bool vflip)
{
	std::shared_ptr<gl::texture> texture;

	// use libjpeg
	FILE *infile;
	if ((infile = fopen(filename.c_str(), "rb")) == NULL)
	{
		log::shadertoy()->error("Could not open {} for reading for input {}", filename);
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
			log::shadertoy()->error("Unsupported component count for JPEG {}", filename);
			// Don't decode unknown format
			decode = false;
		}

		if (decode)
		{
			int stride = cinfo.output_width * cinfo.output_components;
			JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, stride, 1);
			char *imgbuf = new char[cinfo.output_height * stride];

			while (cinfo.output_scanline < cinfo.output_height)
			{
				JDIMENSION read_now = jpeg_read_scanlines(&cinfo, buffer, 1);
				size_t off = cinfo.output_scanline - read_now;
				if (vflip)
					off = cinfo.output_height - off - read_now;
				memcpy(&imgbuf[off * stride], buffer[0], stride);
			}

			texture = std::make_shared<gl::texture>(GL_TEXTURE_2D);
			texture->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, cinfo.output_width, cinfo.output_height,
							  0, fmt, GL_UNSIGNED_BYTE, imgbuf);

			delete[] imgbuf;
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
	}

	return texture;
}

jpeg_input::jpeg_input() : file_input() {}

jpeg_input::jpeg_input(const std::string &filename) : file_input(filename) {}


#include <epoxy/gl.h>

#if LIBSHADERTOY_JPEG
#include <cstdio>
#include <jpeglib.h>
#endif /* LIBSHADERTOY_JPEG */

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/assert.hpp"

#include "shadertoy/inputs/jpeg_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::log;
using shadertoy::utils::error_assert;

std::unique_ptr<gl::texture> jpeg_input::load_file(const std::string &filename, bool vflip)
{
	std::unique_ptr<gl::texture> texture;

#if LIBSHADERTOY_JPEG
	// use libjpeg
	FILE *infile;
	if ((infile = fopen(filename.c_str(), "rbe")) == nullptr)
	{
		error_assert(false, "Cannot load {} for input {}: failed to open file for reading",
					 filename, static_cast<const void *>(this));
	}
	else
	{
		log::shadertoy()->trace("Reading {} for input {}", filename, static_cast<const void *>(this));

		struct jpeg_decompress_struct cinfo
		{
		};
		struct jpeg_error_mgr jerr
		{
		};
		cinfo.err = jpeg_std_error(&jerr);

		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, infile);

		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		GLenum fmt = GL_RGB;
		if (cinfo.output_components == 1)
		{
			fmt = GL_RED;
		}
		else if (cinfo.output_components == 4)
		{
			fmt = GL_RGBA;
		}
		else if (cinfo.output_components != 3)
		{
			// Don't decode unknown format
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);

			error_assert(false, "Cannot load {} for input {}: unsupported component count {}",
						 filename, static_cast<const void *>(this), cinfo.output_components);
		}

		int stride = cinfo.output_width * cinfo.output_components;
		JSAMPARRAY buffer =
		(*cinfo.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&cinfo), JPOOL_IMAGE, stride, 1);
		std::vector<char> imgbuf_vec(cinfo.output_height * stride);
		char *imgbuf(imgbuf_vec.data());

		while (cinfo.output_scanline < cinfo.output_height)
		{
			JDIMENSION read_now = jpeg_read_scanlines(&cinfo, buffer, 1);
			size_t off = cinfo.output_scanline - read_now;
			if (vflip)
			{
				off = cinfo.output_height - off - read_now;
			}
			memcpy(std::addressof(imgbuf[off * stride]), buffer[0], stride);
		}

		texture = std::make_unique<gl::texture>(GL_TEXTURE_2D);
		texture->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, cinfo.output_width, cinfo.output_height,
						  0, fmt, GL_UNSIGNED_BYTE, imgbuf);

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);

		log::shadertoy()->info("Loaded {}x{} JPEG {} for input {} (GL id {})", cinfo.output_width,
							   cinfo.output_height, filename, static_cast<const void *>(this), GLuint(*texture));
	}
#else /* LIBSHADERTOY_JPEG */
	error_assert(false, "Cannot load {} for input {}: JPEG support is disabled", filename,
				 static_cast<const void *>(this));
#endif /* LIBSHADERTOY_JPEG */

	return texture;
}

jpeg_input::jpeg_input() = default;

jpeg_input::jpeg_input(const std::string &filename) : file_input(filename) {}

bool jpeg_input::supported() { return LIBSHADERTOY_JPEG; }

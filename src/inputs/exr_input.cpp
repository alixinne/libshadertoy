#include <epoxy/gl.h>

#if LIBSHADERTOY_OPENEXR
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfRgbaFile.h>
#endif /* LIBSHADERTOY_OPENEXR */

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/assert.hpp"

#include "shadertoy/inputs/exr_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;
using namespace shadertoy::utils;

using shadertoy::utils::log;

std::unique_ptr<gl::texture> exr_input::load_file(const std::string &filename, bool vflip, GLenum &format)
{
	std::unique_ptr<gl::texture> texture;

#if LIBSHADERTOY_OPENEXR
	log::shadertoy()->trace("Reading {} for input {}", filename, static_cast<const void *>(this));

	Imf::RgbaInputFile in(filename.c_str());

	Imath::Box2i win = in.dataWindow();

	Imath::V2i dim(win.max.x - win.min.x + 1, win.max.y - win.min.y + 1);

	std::vector<Imf::Rgba> pixelBuffer(dim.x * dim.y);

	// Set buffer stride according to reading direction
	if (vflip)
	{
		in.setFrameBuffer(std::addressof(pixelBuffer[(dim.y - 1) * dim.x]), 1, -dim.x); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	}
	else
	{
		in.setFrameBuffer(pixelBuffer.data(), 1, dim.x);
	}

	// Read the whole image
	in.readPixels(win.min.y, win.max.y);

	// Create a texture object
	texture = std::make_unique<gl::texture>(GL_TEXTURE_2D);
	texture->image_2d(GL_TEXTURE_2D, 0, format = GL_RGBA16F, dim.x, dim.y, 0, GL_RGBA,
					  GL_HALF_FLOAT, pixelBuffer.data());

	log::shadertoy()->info("Loaded {}x{} EXR {} for input {} (GL id {})", dim.x, dim.y, filename,
						   static_cast<const void *>(this), GLuint(*texture));
#else  /* LIBSHADERTOY_OPENEXR */
	error_assert(false, "Cannot load {} for input {}: OpenEXR support is disabled", filename,
				 static_cast<const void *>(this));
#endif /* LIBSHADERTOY_OPENEXR */

	return texture;
}

exr_input::exr_input() = default;

exr_input::exr_input(const std::string &filename) : file_input(filename) {}

bool exr_input::supported() { return LIBSHADERTOY_OPENEXR; }

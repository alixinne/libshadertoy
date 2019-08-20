#include <epoxy/gl.h>

#include "shadertoy/utils/assert.hpp"

#include "shadertoy/inputs/openimageio_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::log;
using shadertoy::utils::error_assert;

#if LIBSHADERTOY_OPENIMAGEIO
void st_oiio_load_image_to_texture(const std::string &filename, bool vflip, GLenum &output_format, const gl::texture &texture, int &width, int &height);
#endif /* LIBSHADERTOY_OPENIMAGEIO */

std::unique_ptr<gl::texture> openimageio_input::load_file(const std::string &filename, bool vflip, GLenum &output_format)
{
	// Create a texture object
	std::unique_ptr<gl::texture> texture;

#if LIBSHADERTOY_OPENIMAGEIO
	log::shadertoy()->trace("Reading {} for input {}", filename, static_cast<const void *>(this));

	texture = std::make_unique<gl::texture>(GL_TEXTURE_2D);

	try
	{
		int width, height;

		st_oiio_load_image_to_texture(filename, vflip, output_format, *texture, width, height);

		log::shadertoy()->info("Loaded {}x{} OpenImageIO {} for input {} (GL id {})", width, height,
							   filename, static_cast<const void *>(this), GLuint(*texture));
	}
	catch (std::runtime_error &ex)
	{
		texture.reset();

		error_assert(false, "{} for {} while loading {}", ex.what(),
					 filename, static_cast<const void *>(this));
	}
#else
	error_assert(false, "Cannot load {} for input {}: OPENIMAGEIO support is disabled", filename,
				 static_cast<const void *>(this));
#endif /* LIBSHADERTOY_OPENIMAGEIO */

	return texture;
}

openimageio_input::openimageio_input() = default;

openimageio_input::openimageio_input(const std::string &filename) : file_input(filename) {}

bool openimageio_input::supported() { return LIBSHADERTOY_OPENIMAGEIO; }

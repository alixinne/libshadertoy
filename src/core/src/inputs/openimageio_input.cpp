#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/utils/assert.hpp"

#include "shadertoy/inputs/openimageio_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;
using namespace shadertoy::backends;

using shadertoy::utils::error_assert;
using shadertoy::utils::log;

#if SHADERTOY_HAS_OPENIMAGEIO
void st_oiio_load_image_to_texture(const std::string &filename, bool vflip, GLenum &output_format,
								   const gx::texture &texture, int &width, int &height);
#endif /* SHADERTOY_HAS_OPENIMAGEIO */

std::unique_ptr<gx::texture> openimageio_input::load_file(const std::string &filename, bool vflip, GLenum &output_format)
{
	// Create a texture object
	std::unique_ptr<gx::texture> texture;

#if SHADERTOY_HAS_OPENIMAGEIO
	log::shadertoy()->trace("Reading {} for input {}", filename, static_cast<const void *>(this));

	texture = backends::current->make_texture(GL_TEXTURE_2D);

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

		error_assert(false, "{} for {} while loading {}", ex.what(), filename, static_cast<const void *>(this));
	}
#else
	error_assert(false, "Cannot load {} for input {}: OPENIMAGEIO support is disabled", filename,
				 static_cast<const void *>(this));
#endif /* SHADERTOY_HAS_OPENIMAGEIO */

	return texture;
}

openimageio_input::openimageio_input() = default;

openimageio_input::openimageio_input(const std::string &filename) : file_input(filename) {}

bool openimageio_input::supported() { return SHADERTOY_HAS_OPENIMAGEIO; }

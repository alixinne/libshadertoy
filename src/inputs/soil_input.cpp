#include <epoxy/gl.h>

#if LIBSHADERTOY_SOIL
#include <SOIL/SOIL.h>
#endif /* LIBSHADERTOY_SOIL */

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/assert.hpp"

#include "shadertoy/inputs/soil_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::log;
using shadertoy::utils::error_assert;

std::unique_ptr<gl::texture> soil_input::load_file(const std::string &filename, bool vflip, GLenum &format)
{
	// Create a texture object
	std::unique_ptr<gl::texture> texture;
	
#if LIBSHADERTOY_SOIL
	log::shadertoy()->trace("Reading {} for input {}", filename, static_cast<const void *>(this));

	texture = std::make_unique<gl::texture>(GL_TEXTURE_2D);

	// TODO: replace SOIL with openimageio
	//
	// Here, we force the use of GL_RGBA (4 channels). But it uses
	// GL_RGBA internally which means the implementation is free to
	// chose any suitable internal format that matches that. By default
	// it is safe to assume it will end up being GL_RGBA8.
	format = GL_RGBA8;

	// Load image into texture object using SOIL
	GLuint texid = SOIL_load_OGL_texture(filename.c_str(), 4, GLuint(*texture),
										 vflip ? SOIL_FLAG_INVERT_Y : 0);

	if (texid == 0)
	{
		// If loading failed, delete the texture object
		texture = {};
		error_assert(false, "Cannot load {} for input {}: {}", filename, static_cast<const void *>(this),
					 SOIL_last_result());
	}
	else
	{
		GLint width, height;
		texture->get_parameter(0, GL_TEXTURE_WIDTH, &width);
		texture->get_parameter(0, GL_TEXTURE_HEIGHT, &height);

		log::shadertoy()->info("Loaded {}x{} SOIL {} for input {} (GL id {})", width, height,
							   filename, static_cast<const void *>(this), GLuint(*texture));
	}
#else
	error_assert(false, "Cannot load {} for input {}: SOIL support is disabled", filename,
				 static_cast<const void *>(this));
#endif /* LIBSHADERTOY_SOIL */

	return texture;
}

soil_input::soil_input() = default;

soil_input::soil_input(const std::string &filename) : file_input(filename) {}

bool soil_input::supported() { return LIBSHADERTOY_SOIL; }

#include <epoxy/gl.h>

#if LIBSHADERTOY_SOIL
#include <SOIL/SOIL.h>
#endif /* LIBSHADERTOY_SOIL */

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "shadertoy/inputs/soil_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;
using namespace shadertoy::utils;

// Error message from SOIL
extern "C" {
extern char *result_string_pointer;
}

std::unique_ptr<gl::texture> soil_input::load_file(const std::string &filename, bool vflip)
{
	// Create a texture object
	std::unique_ptr<gl::texture> texture;
	
#if LIBSHADERTOY_SOIL
	texture = std::make_unique<gl::texture>(GL_TEXTURE_2D);

	// Load image into texture object using SOIL
	GLuint texid = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, GLuint(*texture),
										 vflip ? SOIL_FLAG_INVERT_Y : 0);

	if (texid == 0)
	{
		log::shadertoy()->error("Failed to load {}: {}", filename, result_string_pointer);

		// If loading failed, delete the texture object
		texture = {};
	}
#else
	log::shadertoy()->error("Cannot load {}: SOIL support is not enabled", filename);
#endif /* LIBSHADERTOY_SOIL */

	return texture;
}

soil_input::soil_input() : file_input() {}

soil_input::soil_input(const std::string &filename) : file_input(filename) {}

bool soil_input::supported() { return LIBSHADERTOY_SOIL; }

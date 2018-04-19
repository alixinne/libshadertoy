#include <cstdio>

#include <epoxy/gl.h>

#include <SOIL/SOIL.h>

#include "shadertoy/pre.hpp"

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

std::shared_ptr<gl::texture> soil_input::load_file(const std::string &filename, bool vflip)
{
	// Create a texture object
	std::shared_ptr<gl::texture> texture(std::make_shared<gl::texture>(GL_TEXTURE_2D));

	// Load image into texture object using SOIL
	GLuint texid = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, GLuint(*texture),
										 vflip ? SOIL_FLAG_INVERT_Y : 0);

	if (texid == 0)
	{
		log::shadertoy()->error("Failed to load {}: {}", filename, result_string_pointer);

		// If loading failed, delete the texture object
		texture = std::shared_ptr<gl::texture>();
	}

	return texture;
}

soil_input::soil_input() : file_input() {}

soil_input::soil_input(const std::string &filename) : file_input(filename) {}

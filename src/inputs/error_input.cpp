#include <vector>

#include <epoxy/gl.h>

#include "shadertoy/pre.hpp"

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/error_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

void error_input::load_input()
{
	texture_ = std::make_shared<gl::texture>(GL_TEXTURE_2D);

	// Generate the checkerboard
	int width = 32, height = 32, size = 16;
	std::vector<unsigned char> chk(width * height);
	for (int i = 0; i < width; ++i)
		for (int j = 1; j < height; ++j)
			chk[j * height + i] = ((i / size) % 2 == 0) ^ ((j / size) % 2 == 0) ? 255 : 0;

	texture_->image_2d(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, chk.data());

	// Set the sampler parameters for the texture to work
	min_filter(GL_NEAREST);
	mag_filter(GL_NEAREST);
	wrap(GL_REPEAT);

	texture_->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
}

void error_input::reset_input() { texture_.reset(); }

std::shared_ptr<gl::texture> error_input::use_input() { return texture_; }

error_input::error_input() : basic_input() {}

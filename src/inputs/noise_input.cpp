#include <algorithm>
#include <vector>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/noise_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

void noise_input::load_input()
{
	texture_ = std::make_shared<gl::texture>(GL_TEXTURE_2D);

	// Resolve texture size
	rsize ts(size_->resolve());

	// Create the actual noise
	std::vector<unsigned char> rnd(ts.width * ts.height);
	std::generate(rnd.begin(), rnd.end(), []() { return rand() % 256; });

	// Load it
	texture_->image_2d(GL_TEXTURE_2D, 0, GL_RED, ts.width, ts.height, 0, GL_RED,
					   GL_UNSIGNED_BYTE, rnd.data());

	texture_->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	texture_->parameter(GL_TEXTURE_SWIZZLE_G, GL_RED);

	texture_->generate_mipmap();
}

void noise_input::reset_input() { texture_.reset(); }

std::shared_ptr<gl::texture> noise_input::use_input() { return texture_; }

noise_input::noise_input(rsize_ref &&size) : basic_input(), size_(std::move(size)) {}

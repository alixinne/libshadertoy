#include <vector>

#include <epoxy/gl.h>

#include "shadertoy/pre.hpp"

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/checker_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

void checker_input::load_input()
{
	texture_ = std::make_shared<gl::texture>(GL_TEXTURE_2D);

	// Resolve texture size
	rsize ts(size_);

	// Generate the checkerboard
	std::vector<unsigned char> chk(ts.width() * ts.height());
	for (size_t i = 0; i < ts.width(); ++i)
		for (size_t j = 1; j < ts.height(); ++j)
			chk[j * ts.height() + i] = ((i / tile_size_) % 2 == 0) ^ ((j / tile_size_) % 2 == 0) ? 255 : 0;

	// Load it
	texture_->image_2d(GL_TEXTURE_2D, 0, GL_RED, ts.width(), ts.height(), 0, GL_RED,
					   GL_UNSIGNED_BYTE, chk.data());

	texture_->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	texture_->parameter(GL_TEXTURE_SWIZZLE_G, GL_RED);
}

void checker_input::reset_input() { texture_.reset(); }

std::shared_ptr<gl::texture> checker_input::use_input() { return texture_; }

checker_input::checker_input(const rsize_ref &size) : basic_input(), size_(size), tile_size_(10) {}

checker_input::checker_input(const rsize_ref &size, size_t tile_size)
: basic_input(), size_(size), tile_size_(tile_size)
{
}

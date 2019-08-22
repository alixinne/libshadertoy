#include <vector>

#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/inputs/checker_input.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::error_assert;
using shadertoy::utils::log;

GLenum checker_input::load_input()
{
	auto format = GL_R8;

	texture_ = backends::current->make_texture(GL_TEXTURE_2D);

	// Resolve texture size
	rsize ts(size_->resolve());

	error_assert(ts.width != 0 && ts.height != 0, "Checkerboard tile size is zero for input {}",
				 static_cast<const void *>(this));

	// Generate the checkerboard
	std::vector<unsigned char> chk(ts.width * ts.height);
	for (size_t i = 0; i < ts.width; ++i)
	{
		for (size_t j = 1; j < ts.height; ++j)
		{
			chk[j * ts.height + i] = ((i / tile_size_) % 2 == 0) ^ ((j / tile_size_) % 2 == 0) ? 255 : 0;
		}
	}

	// Load it
	texture_->image_2d(GL_TEXTURE_2D, 0, format, ts.width, ts.height, 0, GL_RED,
					   GL_UNSIGNED_BYTE, chk.data());

	texture_->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	texture_->parameter(GL_TEXTURE_SWIZZLE_G, GL_RED);

	texture_->generate_mipmap();

	log::shadertoy()->info("Generated {}x{} checkerboard texture for {} (GL id {})", ts.width,
						   ts.height, static_cast<const void *>(this), GLuint(*texture_));

	return format;
}

void checker_input::reset_input() { texture_.reset(); }

backends::gx::texture *checker_input::use_input() { return texture_.get(); }

checker_input::checker_input(rsize_ref size) : size_(std::move(size)), tile_size_(10) {}

checker_input::checker_input(rsize_ref size, size_t tile_size)
: size_(std::move(size)), tile_size_(tile_size)
{
}

#include "shadertoy/backends/gx/backend.hpp"

#include <algorithm>
#include <random>
#include <vector>

#include "shadertoy/inputs/noise_input.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::error_assert;
using shadertoy::utils::log;

GLenum noise_input::load_input()
{
	auto format = GL_R8;

	texture_ = backends::current()->make_texture(GL_TEXTURE_2D);

	// Resolve texture size
	rsize ts(size_->resolve());

	error_assert(ts.width != 0 && ts.height != 0, "Noise tile size is zero for input {}",
				 static_cast<const void *>(this));

	// Create the actual noise
	std::random_device hr;
	std::minstd_rand r(hr());
	std::uniform_int_distribution<uint_fast32_t> uniform_dist(0, 255);

	std::vector<unsigned char> rnd(ts.width * ts.height);
	std::generate(rnd.begin(), rnd.end(), [&]() { return uniform_dist(r); });

	// Load it
	texture_->image_2d(GL_TEXTURE_2D, 0, format, ts.width, ts.height, 0, GL_RED, GL_UNSIGNED_BYTE,
					   rnd.data());

	texture_->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	texture_->parameter(GL_TEXTURE_SWIZZLE_G, GL_RED);

	texture_->generate_mipmap();

	log::shadertoy()->info("Generated {}x{} noise texture for {} (GL id {})", ts.width, ts.height,
						   static_cast<const void *>(this), GLuint(*texture_));

	return format;
}

void noise_input::reset_input() { texture_.reset(); }

backends::gx::texture *noise_input::use_input() { return texture_.get(); }

noise_input::noise_input(rsize_ref size) : size_(std::move(size)) {}

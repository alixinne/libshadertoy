#include <vector>

#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/utils/log.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::log;

GLenum error_input::load_input()
{
	auto format = GL_R8;

	texture_ = backends::current->make_texture(GL_TEXTURE_2D);

	// Generate the checkerboard
	int width = 32, height = 32, size = 16;
	std::vector<unsigned char> chk(width * height);
	for (int i = 0; i < width; ++i)
	{
		for (int j = 1; j < height; ++j)
		{
			chk[j * height + i] = ((i / size) % 2 == 0) ^ ((j / size) % 2 == 0) ? 255 : 0;
		}
	}

	texture_->image_2d(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, chk.data());

	// Set the sampler parameters for the texture to work
	min_filter(GL_NEAREST);
	mag_filter(GL_NEAREST);
	wrap(GL_REPEAT);

	texture_->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);

	texture_->generate_mipmap();

	log::shadertoy()->trace("Initialized error input at {} (GL id {})",
							static_cast<const void *>(this), GLuint(*texture_));

	return format;
}

void error_input::reset_input() { texture_.reset(); }

backends::gx::texture *error_input::use_input() { return texture_.get(); }

error_input::error_input() = default;

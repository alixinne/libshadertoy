#include <epoxy/gl.h>

#include "shadertoy/pre.hpp"

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/image_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

void image_input::load_input()
{
	image_texture_ = load_image();

	// Note that we always generate mipmaps, to allow changing
	// the sampler settings afterwards from non-mipmap to mipmap
	if (image_texture_)
	{
		image_texture_->generate_mipmap();
	}
}

void image_input::reset_input() { image_texture_.reset(); }

std::shared_ptr<gl::texture> image_input::use_input() { return image_texture_; }

image_input::image_input() : basic_input(), image_texture_() {}

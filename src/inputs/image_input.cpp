#include <epoxy/gl.h>

#include "shadertoy/inputs/image_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

GLenum image_input::load_input()
{
	GLenum format = 0;

	image_texture_ = load_image(format);

	// Note that we always generate mipmaps, to allow changing
	// the sampler settings afterwards from non-mipmap to mipmap
	if (image_texture_)
	{
		image_texture_->generate_mipmap();
	}

	return format;
}

void image_input::reset_input() { image_texture_.reset(); }

gl::texture *image_input::use_input() { return image_texture_.get(); }

image_input::image_input() : image_texture_() {}

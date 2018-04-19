#include <memory>

#include <epoxy/gl.h>

#include "shadertoy/pre.hpp"

#include "shadertoy/gl.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"
#include "shadertoy/inputs/buffer_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

void buffer_input::load_input() {}

void buffer_input::reset_input() {}

std::shared_ptr<gl::texture> buffer_input::use_input()
{
	if (buffer_)
	{
		auto tex(buffer_->source_texture());

		// TODO: a buffer not currently in use in a swap chain
		// might not need updating its mipmaps
		if (min_filter() > GL_LINEAR)
		{
			tex->generate_mipmap();
		}

		return tex;
	}
	return std::shared_ptr<gl::texture>();
}

buffer_input::buffer_input() : buffer_() {}

buffer_input::buffer_input(std::shared_ptr<buffers::basic_buffer> buffer) : buffer_(buffer) {}

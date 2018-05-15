#include <memory>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/inputs/buffer_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

void buffer_input::load_input() {}

void buffer_input::reset_input() {}

std::shared_ptr<gl::texture> buffer_input::use_input()
{
	if (member_)
	{
		auto tex(member_->output());

		// TODO: a member not currently in use in a swap chain
		// might not need updating its mipmaps
		if (min_filter() > GL_LINEAR)
		{
			tex->generate_mipmap();
		}

		return tex;
	}

	return {};
}

buffer_input::buffer_input()
	: member_()
{}

buffer_input::buffer_input(std::shared_ptr<members::basic_member> member)
	: member_(member)
{}

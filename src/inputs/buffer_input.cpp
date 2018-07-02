#include <memory>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/inputs/buffer_input.hpp"

#include "shadertoy/utils/log.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::log;

void buffer_input::load_input() {}

void buffer_input::reset_input() {}

gl::texture *buffer_input::use_input()
{
	if (auto member = member_.lock())
	{
		auto tex(member->output());

		// TODO: a member not currently in use in a swap chain
		// might not need updating its mipmaps
		if (min_filter() > GL_LINEAR)
		{
			tex->generate_mipmap();
		}

		return tex;
	}
	else
	{
		log::shadertoy()->warn("Failed to acquire pointer to member from input {}", (void*)this);
	}

	return {};
}

buffer_input::buffer_input()
	: member_()
{}

buffer_input::buffer_input(std::weak_ptr<members::basic_member> member)
	: member_(member)
{}

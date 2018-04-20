#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/uniform_state.hpp"

#include "shadertoy/render_context.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"

#include "shadertoy/members/buffer_member.hpp"

using namespace shadertoy;
using namespace shadertoy::members;

void buffer_member::render_member(swap_chain &chain, render_context &context)
{
	buffer_->render(context);
}

void buffer_member::init_member(swap_chain &chain, render_context &context)
{
	buffer_->init(context);
}

void buffer_member::allocate_member(swap_chain &chain, render_context &context)
{
	buffer_->allocate_textures(context);
}

buffer_member::buffer_member(std::shared_ptr<buffers::basic_buffer> buffer)
	: buffer_(buffer)
{
}

std::shared_ptr<gl::texture> buffer_member::output(swap_chain &chain)
{
	return buffer_->source_texture();
}

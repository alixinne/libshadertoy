#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/uniform_state.hpp"

#include "shadertoy/render_context.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"

#include "shadertoy/members/buffer_member.hpp"

using namespace shadertoy;
using namespace shadertoy::members;

void buffer_member::render_member(const swap_chain &chain, const render_context &context)
{
	buffer_->render(context, io_);

	// Swap texture object pointers
	io_.swap();
}

void buffer_member::init_member(const swap_chain &chain, const render_context &context)
{
	buffer_->init(context, io_);
}

void buffer_member::allocate_member(const swap_chain &chain, const render_context &context)
{
	// Initialize buffer textures
	io_.allocate();

	buffer_->allocate_textures(context, io_);
}

buffer_member::buffer_member(std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size)
	: buffer_(buffer),
	io_(std::forward<rsize_ref&&>(render_size))
{
}

std::shared_ptr<gl::texture> buffer_member::output()
{
	return io_.source_texture();
}

std::shared_ptr<buffer_member> members::member_data(std::shared_ptr<buffers::basic_buffer>&& buffer, rsize_ref &&render_size)
{ return std::make_shared<buffer_member>(buffer, std::forward<rsize_ref&&>(render_size)); }

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/uniform_state.hpp"

#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/members/screen_member.hpp"

#include "shadertoy/swap_chain.hpp"
#include "shadertoy/render_context.hpp"

using namespace shadertoy;
using namespace shadertoy::members;

using shadertoy::gl::gl_call;

void screen_member::render_member(const swap_chain &chain, const render_context &context)
{
	auto texptr(output(chain));

	rsize vp_size(viewport_size_->resolve());
	gl_call(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);
	gl_call(glViewport, viewport_x_, viewport_y_, vp_size.width, vp_size.height);

	// Use the screen program
	context.screen_prog().use();

	// Bind the texture and sampler
	texptr->bind_unit(0);
	sampler_.bind(0);

	context.render_screen_quad();
}

void screen_member::init_member(const swap_chain &chain, const render_context &context)
{
}

void screen_member::allocate_member(const swap_chain &chain, const render_context &context)
{
}

std::shared_ptr<gl::texture> screen_member::output(const swap_chain &chain)
{
	if (member_)
	{
		auto out(member_->output());
		assert(out);
		return out;
	}

	// Check that the swapchain has a last rendered-to member
	auto before(chain.before(this));
	assert(before);

	auto texptr(before->output());
	assert(texptr);

	return texptr;
}

screen_member::screen_member(rsize_ref &&viewport_size)
	: sampler_(),
	viewport_x_(0),
	viewport_y_(0),
	viewport_size_(std::move(viewport_size))
{
	sampler_.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

std::shared_ptr<gl::texture> screen_member::output()
{
	if (member_)
		return member_->output();

	return {};
}

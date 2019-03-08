#include <epoxy/gl.h>

#include <utility>

#include "shadertoy/gl.hpp"
#include "shadertoy/uniform_state.hpp"

#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/members/screen_member.hpp"

#include "shadertoy/render_context.hpp"
#include "shadertoy/swap_chain.hpp"

#include "shadertoy/geometry/screen_quad.hpp"

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

	// Apply member state
	state_.apply();

	// Clear buffers as requested
	state_.clear();

	context.screen_quad().render();
}

void screen_member::init_member(const swap_chain &chain, const render_context &context)
{
}

void screen_member::allocate_member(const swap_chain &chain, const render_context &context)
{
}

gl::texture *screen_member::output(const swap_chain &chain)
{
	if (auto member = member_.lock())
	{
		auto out(member->output());
		assert(out != nullptr);
		return out;
	}

	// Check that the swapchain has a last rendered-to member
	auto before(chain.before(this));
	assert(before != nullptr);

	auto texptr(before->output());
	assert(texptr != nullptr);

	return texptr;
}

screen_member::screen_member(rsize_ref &&viewport_size)
: viewport_x_(0), viewport_y_(0), viewport_size_(std::move(viewport_size))
{
	sampler_.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

screen_member::screen_member(int viewport_x, int viewport_y, rsize_ref &&viewport_size)
: viewport_x_(viewport_x), viewport_y_(viewport_y), viewport_size_(std::move(viewport_size))
{
	sampler_.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

screen_member::screen_member(rsize_ref &&viewport_size, std::weak_ptr<members::basic_member> member)
: member_(std::move(std::move(member))),

  viewport_x_(0), viewport_y_(0), viewport_size_(std::move(viewport_size))
{
	sampler_.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

screen_member::screen_member(int viewport_x, int viewport_y, rsize_ref &&viewport_size,
							 std::weak_ptr<members::basic_member> member)
: member_(std::move(std::move(member))),

  viewport_x_(viewport_x), viewport_y_(viewport_y), viewport_size_(std::move(viewport_size))
{
	sampler_.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

gl::texture *screen_member::output()
{
	if (auto member = member_.lock())
	{
		return member->output();
	}

	return nullptr;
}

#include "shadertoy/backends/gx/backend.hpp"
#include "shadertoy/utils/assert.hpp"
#include "shadertoy/utils/log.hpp"

#include <utility>

#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/members/screen_member.hpp"

#include "shadertoy/render_context.hpp"
#include "shadertoy/swap_chain.hpp"

#include "shadertoy/geometry/screen_quad.hpp"

using namespace shadertoy;
using namespace shadertoy::members;

using shadertoy::utils::error_assert;
using shadertoy::utils::log;

void screen_member::render_member(const swap_chain &chain, const render_context &context)
{
	backends::gx::texture *texptr = nullptr;
	auto outputs(output(chain));

	if (output_name_)
	{
		// Check that the output id is valid
		if (output_index_ >= 0)
		{
			if (static_cast<size_t>(output_index_) >= outputs.size() ||
				std::get<0>(outputs[output_index_]) != *output_name_)
				output_index_ = -1;
		}

		// Try to find the output
		if (output_index_ < 0)
		{
			for (auto it = outputs.begin(); it != outputs.end(); ++it)
			{
				if (std::get<0>(*it) == *output_name_)
				{
					output_index_ = it - outputs.begin();
					break;
				}
			}
		}

		// Load output if it is valid
		if (output_index_ >= 0)
		{
			texptr = std::get<1>(outputs[output_index_]);
		}
		else
		{
			std::visit(
			[&](const auto &name) {
				log::shadertoy()->warn("Failed to find target output at index {} for screen member "
									   "{}",
									   name, static_cast<const void *>(this));
			},
			*output_name_);
		}
	}
	else
	{
		texptr = std::get<1>(outputs.front());
	}

	rsize vp_size(viewport_size_->resolve());
	backends::current()->bind_default_framebuffer(GL_DRAW_FRAMEBUFFER);
	backends::current()->set_viewport(viewport_x_, viewport_y_, vp_size.width, vp_size.height);

	// Use the screen program
	context.screen_prog().use();

	// Bind the texture and sampler
	texptr->bind_unit(0);
	sampler_->bind(0);
	backends::current()->unbind_texture_units(1);

	// Apply member state
	state_->apply();

	// Clear buffers as requested
	state_->clear();

	context.screen_quad().render();
}

void screen_member::init_member(const swap_chain &chain, const render_context &context) {}

void screen_member::allocate_member(const swap_chain &chain, const render_context &context) {}

std::vector<member_output_t> screen_member::output(const swap_chain &chain)
{
	if (auto member = member_.lock())
	{
		auto out(member->output());
		error_assert(!out.empty(), "Member {} used as input for screen_member does not have any rendered-to targets for screen_member {}",
					 static_cast<const void *>(member.get()), static_cast<const void *>(this));
		return out;
	}

	// Check that the swapchain has a last rendered-to member
	auto before(chain.before(this));
	error_assert(before != nullptr, "Swap chain {} has no member preceding screen_member {}",
				 static_cast<const void *>(&chain), static_cast<const void *>(this));

	auto texptr(before->output());
	error_assert(!texptr.empty(), "Preceding member {} used as input for screen_member does not have any rendered-to targets for screen_member {}",
				 static_cast<const void *>(before.get()), static_cast<const void *>(this));

	return texptr;
}

screen_member::screen_member(rsize_ref &&viewport_size, std::optional<output_name_t> output_name)
: output_name_(output_name), output_index_(-1), sampler_(backends::current()->make_sampler()),
  viewport_x_(0), viewport_y_(0), viewport_size_(std::move(viewport_size)),
  state_(backends::current()->make_draw_state())
{
	sampler_->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

screen_member::screen_member(int viewport_x, int viewport_y, rsize_ref &&viewport_size,
							 std::optional<output_name_t> output_name)
: output_name_(output_name), output_index_(-1), sampler_(backends::current()->make_sampler()),
  viewport_x_(viewport_x), viewport_y_(viewport_y), viewport_size_(std::move(viewport_size)),
  state_(backends::current()->make_draw_state())
{
	sampler_->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

screen_member::screen_member(rsize_ref &&viewport_size, std::weak_ptr<members::basic_member> member,
							 std::optional<output_name_t> output_name)
: member_(std::move(std::move(member))), output_name_(output_name), output_index_(-1),
  sampler_(backends::current()->make_sampler()),

  viewport_x_(0), viewport_y_(0), viewport_size_(std::move(viewport_size)),
  state_(backends::current()->make_draw_state())
{
	sampler_->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

screen_member::screen_member(int viewport_x, int viewport_y, rsize_ref &&viewport_size,
							 std::weak_ptr<members::basic_member> member, std::optional<output_name_t> output_name)
: member_(std::move(std::move(member))), output_name_(output_name), output_index_(-1),
  sampler_(backends::current()->make_sampler()),

  viewport_x_(viewport_x), viewport_y_(viewport_y), viewport_size_(std::move(viewport_size)),
  state_(backends::current()->make_draw_state())
{
	sampler_->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	sampler_->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sampler_->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	sampler_->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

std::vector<member_output_t> screen_member::output()
{
	if (auto member = member_.lock())
	{
		return member->output();
	}

	return {};
}

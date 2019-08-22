#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/backends/gx/texture.hpp"

#include "shadertoy/members/basic_member.hpp"
#include "shadertoy/members/buffer_member.hpp"

#include "shadertoy/swap_chain.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;

using shadertoy::utils::error_assert;

swap_chain::swap_chain()
: internal_format_(GL_RGBA32F), swap_policy_(member_swap_policy::double_buffer)
{
}

swap_chain::swap_chain(GLint internal_format)
: internal_format_(internal_format), swap_policy_(member_swap_policy::double_buffer)
{
}

swap_chain::swap_chain(GLint internal_format, member_swap_policy swap_policy)
: internal_format_(internal_format), swap_policy_(swap_policy)
{
}

std::shared_ptr<members::basic_member> swap_chain::before(members::basic_member *member) const
{
	bool is_next = false;

	for (auto it = members_.crbegin(); it != members_.crend(); ++it)
	{
		if (is_next)
		{
			return *it;
		}
		if ((*it).get() == member)
		{
			is_next = true;
		}
	}

	return {};
}

void swap_chain::push_back(const std::shared_ptr<members::basic_member> &member)
{
	error_assert(members_set_.count(member) == 0, "This member is already part of chain {}",
				 static_cast<const void *>(this));

	members_.push_back(member);
	members_set_.insert(member);
}

std::shared_ptr<members::basic_member> swap_chain::render(const render_context &context)
{
	current_.reset();

	for (auto &member : members_)
	{
		member->render(*this, context);
		current_ = member;
	}

	return current_;
}

std::shared_ptr<members::basic_member>
swap_chain::render(const render_context &context, const std::shared_ptr<members::basic_member> &begin,
				   const std::shared_ptr<members::basic_member> &end)
{
	auto begin_it = std::find(members_.begin(), members_.end(), begin);
	auto end_it = std::find(members_.begin(), members_.end(), end);

	if (begin_it == members_.end())
	{
		throw shadertoy_error("member for beginning render was not found in this swap chain");
	}

	if (end_it == members_.end())
	{
		throw shadertoy_error("member for ending render was not found in this swap chain");
	}

	// We need it to be one past the target member
	end_it++;

	for (auto it = begin_it; it != end_it; ++it)
	{
		(*it)->render(*this, context);
		current_ = *it;
	}

	return current_;
}

void swap_chain::init(const render_context &context)
{
	for (auto &member : members_)
	{
		member->init(*this, context);
	}
}

void swap_chain::allocate_textures(const render_context &context)
{
	for (auto &member : members_)
	{
		member->allocate(*this, context);
	}
}

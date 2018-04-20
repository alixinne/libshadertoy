#include <cassert>

#include "shadertoy/members/basic_member.hpp"
#include "shadertoy/members/buffer_member.hpp"

#include "shadertoy/swap_chain.hpp"

using namespace shadertoy;

swap_chain::swap_chain()
	: members_(),
	members_set_(),
	current_()
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
		else if ((*it).get() == member)
		{
			is_next = true;
		}
	}

	return std::shared_ptr<members::basic_member>();
}

void swap_chain::push_back(std::shared_ptr<members::basic_member> member)
{
	assert(members_set_.count(member) == 0);

	members_.push_back(member);
	members_set_.insert(member);
}

std::shared_ptr<members::basic_member> swap_chain::render(render_context &context,
														  std::shared_ptr<members::basic_member> target)
{
	current_.reset();

	for (auto &member : members_)
	{
		member->render(*this, context);
		current_ = member;

		if (target && current_ == target)
			break;
	}

	return current_;
}

void swap_chain::init(render_context &context)
{
	for (auto &member : members_)
	{
		member->init(*this, context);
	}
}

void swap_chain::allocate_textures(render_context &context)
{
	for (auto &member : members_)
	{
		member->allocate_textures(*this, context);
	}
}

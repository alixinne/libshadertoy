#include <memory>
#include <utility>

#include <epoxy/gl.h>

#include "shadertoy/inputs/buffer_input.hpp"
#include "shadertoy/members/buffer_member.hpp"

#include "shadertoy/utils/log.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::log;

/// Find the target output
std::optional<members::member_output_t> buffer_input::find_output(std::shared_ptr<members::basic_member> member)
{
	auto outputs(member->output());

	// Check that the output id is valid
	if (output_index_ >= 0)
	{
		if (static_cast<size_t>(output_index_) >= outputs.size() || std::get<0>(outputs[output_index_]) != output_name_)
			output_index_ = -1;
	}

	// Try to find the output
	if (output_index_ < 0)
	{
		output_index_ =
		std::visit([&member](const auto &name) { return member->find_output(name); }, output_name_);
	}

	// Load output if it is valid
	if (output_index_ >= 0)
	{
		return outputs[output_index_];
	}

	std::visit(
	[&](const auto &name) {
		log::shadertoy()->warn("Failed to find target output at index {} for input {}", name,
							   static_cast<const void *>(this));
	},
	output_name_);

	return std::nullopt;
}

GLenum buffer_input::load_input()
{
	if (auto member = member_.lock())
	{
		auto output(find_output(member));

		if (output != std::nullopt)
		{
			return std::get<2>(*output);
		}

		return 0;
	}

	log::shadertoy()->warn("Failed to acquire pointer to member from input {}",
						   static_cast<const void *>(this));
	return 0;
}

void buffer_input::reset_input() {}

gl::texture *buffer_input::use_input()
{
	if (auto member = member_.lock())
	{
		auto output(find_output(member));

		if (output != std::nullopt)
		{
			auto tex(std::get<1>(*output));

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

	log::shadertoy()->warn("Failed to acquire pointer to member from input {}",
						   static_cast<const void *>(this));
	return {};
}

buffer_input::buffer_input() : output_name_(0), output_index_(-1) {}

buffer_input::buffer_input(std::weak_ptr<members::basic_member> member, output_name_t output_name)
: member_(std::move(std::move(member))), output_name_(output_name), output_index_(-1)
{
}

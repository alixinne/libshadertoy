#include <epoxy/gl.h>

#include <algorithm>
#include <utility>

#include "shadertoy/gl.hpp"

#include "shadertoy/render_context.hpp"
#include "shadertoy/uniform_state.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"

#include "shadertoy/members/buffer_member.hpp"

#include "shadertoy/swap_chain.hpp"

using namespace shadertoy;
using namespace shadertoy::members;

using shadertoy::utils::log;

void buffer_member::render_member(const swap_chain &chain, const render_context &context)
{
	buffer_->render(context, io_, *this);

	// Swap texture object pointers
	io_.swap();
}

void buffer_member::init_member(const swap_chain &chain, const render_context &context)
{
	buffer_->init(context, io_);

	// Add discovered outputs
	auto outputs(buffer_->get_buffer_outputs());
	auto &specs(io_.output_specs());

	if (outputs)
	{
		// Make sure there are enough spec objects
		specs.resize(outputs->size());

		// Emplace updated outputs
		auto it_output = outputs->begin();
		auto it_spec = specs.begin();

		for (; it_output != outputs->end(); ++it_output, ++it_spec)
		{
			auto name(std::make_tuple(it_output->name, it_output->location));

			if (output_allocator_)
			{
				auto props(output_allocator_(*it_output, render_size_));
				*it_spec = output_buffer_spec(std::move(std::get<0>(props)), name, std::get<1>(props));
			}
			else
			{
				*it_spec = output_buffer_spec(make_ref(*render_size_), name, internal_format_);
			}
		}
	}
	else
	{
		log::shadertoy()->debug("Buffer {} does not return discovered outputs, they have to be "
								"setup manually before the allocation step");
	}
}

void buffer_member::allocate_member(const swap_chain &chain, const render_context &context)
{
	// Initialize buffer textures
	io_.allocate();

	buffer_->allocate_textures(context, io_);
}

buffer_member::buffer_member(std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref render_size,
							 GLint internal_format, member_swap_policy swap_policy)
: buffer_(std::move(std::move(buffer))), io_(swap_policy), render_size_(std::move(render_size)),
  internal_format_(internal_format)
{
}

std::vector<member_output_t> buffer_member::output()
{
	std::vector<member_output_t> result;
	result.resize(io_.output_specs().size());

	for (auto it = io_.output_specs().begin(); it != io_.output_specs().end(); ++it)
	{
		size_t i = it - io_.output_specs().begin();
		result[i] = std::make_tuple(it->name, io_.source_texture(i).get());
	}

	return result;
}

int buffer_member::find_output(const output_name_t &name) const
{
	auto it = std::find_if(io_.output_specs().begin(), io_.output_specs().end(),
						   [&name](const auto &item) { return item.name == name; });

	if (it == io_.output_specs().end())
		return -1;

	return it - io_.output_specs().end();
}

std::shared_ptr<buffer_member> members::make_member(const swap_chain &chain, std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size)
{
	return make_buffer(buffer, std::forward<rsize_ref&&>(render_size), chain.internal_format(), chain.swap_policy());
}

std::shared_ptr<buffer_member> members::make_member(const swap_chain &chain, std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size, GLint internal_format)
{
	return make_buffer(buffer, std::forward<rsize_ref&&>(render_size), internal_format, chain.swap_policy());
}

std::shared_ptr<buffer_member> members::make_member(const swap_chain &chain, std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size, member_swap_policy swap_policy)
{
	return make_buffer(buffer, std::forward<rsize_ref&&>(render_size), chain.internal_format(), swap_policy);
}

std::shared_ptr<buffer_member> members::make_member(const swap_chain &chain, std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size, GLint internal_format, member_swap_policy swap_policy)
{
	return make_buffer(buffer, std::forward<rsize_ref&&>(render_size), internal_format, swap_policy);
}

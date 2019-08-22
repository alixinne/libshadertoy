#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/members/compute_member.hpp"

using namespace shadertoy;
using namespace shadertoy::members;

void compute_member::render_member(const swap_chain &chain, const render_context &context)
{
	buffer_->dispatch(context);
}

void compute_member::init_member(const swap_chain &chain, const render_context &context)
{
	buffer_->init(context);
}

void compute_member::allocate_member(const swap_chain &chain, const render_context &context)
{
	buffer_->allocate_textures(context);
}

compute_member::compute_member(std::shared_ptr<buffers::compute_buffer> buffer) : buffer_(buffer) {}

std::vector<member_output_t> compute_member::output() { return {}; }

int compute_member::find_output(const output_name_t &name) const { return -1; }

std::shared_ptr<compute_member>
members::make_member(const swap_chain &chain, std::shared_ptr<buffers::compute_buffer> buffer)
{
	return make_compute(buffer);
}

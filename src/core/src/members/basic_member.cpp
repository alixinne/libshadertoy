#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/members/basic_member.hpp"

using namespace shadertoy;
using namespace shadertoy::members;

void basic_member::render(const swap_chain &chain, const render_context &context)
{
	render_member(chain, context);
}

void basic_member::init(const swap_chain &chain, const render_context &context)
{
	init_member(chain, context);
}

void basic_member::allocate(const swap_chain &chain, const render_context &context)
{
	allocate_member(chain, context);
}

int basic_member::find_output(const output_name_t &name) const { return -1; }

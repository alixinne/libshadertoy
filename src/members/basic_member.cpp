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

void basic_member::allocate_textures(const swap_chain &chain, const render_context &context)
{
	allocate_member(chain, context);
}

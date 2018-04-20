#include "shadertoy/members/basic_member.hpp"

using namespace shadertoy;
using namespace shadertoy::members;

void basic_member::render(swap_chain &chain, render_context &context)
{
	render_member(chain, context);
}

void basic_member::init(swap_chain &chain, render_context &context)
{
	init_member(chain, context);
}

void basic_member::allocate_textures(swap_chain &chain, render_context &context)
{
	allocate_member(chain, context);
}

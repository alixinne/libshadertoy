#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/toy_buffer.hpp"
#include "shadertoy/render_context.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;
using shadertoy::gl::gl_call;

toy_buffer::toy_buffer(const std::string &id)
	: program_buffer(id)
{
}

void toy_buffer::init_geometry(const render_context &context, const io_resource &io)
{
	// Nothing to do, the geometry is managed by context
}

void toy_buffer::render_geometry(const render_context &context, const io_resource &io)
{
	// Render the program on a quad
	context.render_screen_quad(time_delta_query());
}


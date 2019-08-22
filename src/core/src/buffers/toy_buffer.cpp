#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/buffers/toy_buffer.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/geometry/screen_quad.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

toy_buffer::toy_buffer(const std::string &id) : program_buffer(id) {}

void toy_buffer::init_geometry(const render_context &context, const io_resource &io)
{
	// Just access the quad geometry so it is loaded now instead of during rendering
	context.screen_quad();
}

void toy_buffer::render_geometry(const render_context &context, const io_resource &io)
{
	// Render the program on a quad
	context.screen_quad().render(time_delta_query());
}

#include <epoxy/gl.h>

#include <utility>

#include "shadertoy/gl.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"
#include "shadertoy/render_context.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

basic_buffer::basic_buffer(std::string id) : id_(std::move(id)), time_delta_query_(GL_TIME_ELAPSED)
{
}

void basic_buffer::init(const render_context &context, const io_resource &io)
{
	// Initialize buffer contents
	init_contents(context, io);
}

void basic_buffer::allocate_textures(const render_context &context, const io_resource &io)
{
	// Allocate content resources
	allocate_contents(context, io);
}

void basic_buffer::render(const render_context &context, const io_resource &io, const members::buffer_member &member)
{
	// Render the contents of this buffer
	render_contents(context, io, member);
}

uint64_t basic_buffer::elapsed_time()
{
	GLint available = 0;

	// Wait for result to be available
	while (available == 0)
	{
		time_delta_query_.get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
	}

	// Fetch result
	GLuint64 result;
	time_delta_query_.get_object_ui64v(GL_QUERY_RESULT, &result);

	return result;
}

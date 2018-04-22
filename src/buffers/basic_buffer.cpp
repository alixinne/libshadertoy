#include <fstream>
#include <map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/basic_buffer.hpp"
#include "shadertoy/render_context.hpp"

using namespace std;
namespace fs = boost::filesystem;

using namespace shadertoy;
using namespace shadertoy::buffers;
using shadertoy::gl::gl_call;

basic_buffer::basic_buffer(const std::string &id)
	: id_(id),
	  time_delta_query_(GL_TIME_ELAPSED)
{
}

void basic_buffer::init(render_context &context, io_resource &io)
{
	// Allocate render textures
	allocate_textures(context, io);

	// Initialize buffer contents
	init_contents(context, io);
}

void basic_buffer::allocate_textures(render_context &context, io_resource &io)
{
	// Initialize buffer textures
	io.allocate();

	// Allocate content resources
	allocate_contents(context, io);
}

void basic_buffer::render(render_context &context, io_resource &io)
{
	// Render the contents of this buffer
	render_contents(context, io);

	// Swap texture object pointers
	io.swap();
}

unsigned long long basic_buffer::elapsed_time()
{
	GLint available = 0;

	// Wait for result to be available
	while (!available)
	{
		time_delta_query_.get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
	}

	// Fetch result
	GLuint64 result;
	time_delta_query_.get_object_ui64v(GL_QUERY_RESULT, &result);

	return result;
}

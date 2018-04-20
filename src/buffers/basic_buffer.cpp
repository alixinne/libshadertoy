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

void basic_buffer::init(render_context &context)
{
	// Allocate render textures
	allocate_textures(context);

	// Initialize buffer contents
	init_contents(context);
}

void basic_buffer::allocate_textures(render_context &context)
{
	// Initialize buffer textures
	init_render_texture(context, source_tex_);
	init_render_texture(context, target_tex_);

	// Allocate content resources
	allocate_contents(context);
}

void basic_buffer::render(render_context &context)
{
	// Render the contents of this buffer
	render_contents(context);

	// Swap texture object pointers
	swap(source_tex_, target_tex_);
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

void basic_buffer::init_render_texture(render_context &context, shared_ptr<gl::texture> &texptr)
{
	// Only create a texture object if it is necessary
	if (!texptr)
		texptr = make_shared<gl::texture>(GL_TEXTURE_2D);

	// Resolve rendering size
	rsize size(render_size_.resolve(context.render_size()));
	// Allocate texture storage according to width/height
	texptr->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, size.width(), size.height(), 0, GL_BGRA,
					 GL_UNSIGNED_BYTE, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	float black[4] = {0.f};
	texptr->clear_tex_image(0, GL_BGRA, GL_FLOAT, black);
}

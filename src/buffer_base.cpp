#include <fstream>
#include <map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl.hpp"

#include "shadertoy/buffer_config.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffer_base.hpp"
#include "shadertoy/texture_engine.hpp"
#include "shadertoy/render_context.hpp"

using namespace std;
namespace fs = boost::filesystem;

using namespace shadertoy;
using shadertoy::gl::gl_call;

buffer_base::buffer_base(const std::string &id)
	: id_(id),
	  time_delta_query_(GL_TIME_ELAPSED)
{
}

void buffer_base::init(render_context &context, int width, int height)
{
	// Allocate render textures
    allocate_textures(width, height);

	// Initialize buffer contents
	init_contents(context, width, height);
}

void buffer_base::allocate_textures(int width, int height)
{
	// Initialize buffer textures
    init_render_texture(source_tex_, width, height);
    init_render_texture(target_tex_, width, height);

	// Setup render buffers
	target_tex_->bind(GL_TEXTURE_2D);
	target_rbo_.bind(GL_RENDERBUFFER);
	target_rbo_.storage(GL_DEPTH_COMPONENT, width, height);
}

void buffer_base::render(render_context &context)
{
	// Update renderbuffer to use the correct target texture and bind as the curren target
	target_tex_->bind(GL_TEXTURE_2D);
	target_rbo_.bind(GL_RENDERBUFFER);
	target_fbo_.bind(GL_DRAW_FRAMEBUFFER);

	target_fbo_.texture(GL_COLOR_ATTACHMENT0, *target_tex_, 0);
	
	// Render the contents of this buffer
	render_contents(context);

	// Swap texture object pointers
	swap(source_tex_, target_tex_);
}

unsigned long long buffer_base::elapsed_time()
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

void buffer_base::init_render_texture(shared_ptr<gl::texture> &texptr, int width, int height)
{
	// Only create a texture object if it is necessary
	if (!texptr)
		texptr = make_shared<gl::texture>(GL_TEXTURE_2D);

	// Allocate texture storage according to width/height
	texptr->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	float black[4] = {0.f};
	texptr->clear_tex_image(0, GL_BGRA, GL_FLOAT, black);
}

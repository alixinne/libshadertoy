#include <fstream>
#include <map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/buffer_config.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/gl_buffer.hpp"
#include "shadertoy/texture_engine.hpp"
#include "shadertoy/render_context.hpp"

using namespace std;
namespace fs = boost::filesystem;

using namespace shadertoy;
using namespace shadertoy::buffers;
using shadertoy::gl::gl_call;

gl_buffer::gl_buffer(const std::string &id)
	: basic_buffer(id)
{
}

void gl_buffer::init_contents(render_context &context)
{
	// Nothing currently
}

void gl_buffer::allocate_contents(render_context &context)
{
	// Setup render buffers
	target_texture()->bind(GL_TEXTURE_2D);
	target_rbo_.bind(GL_RENDERBUFFER);

	// Resolve size
	rsize size(render_size());
	target_rbo_.storage(GL_DEPTH_COMPONENT, size.width(), size.height());
}

void gl_buffer::render_contents(render_context &context)
{
	// Update renderbuffer to use the correct target texture and bind as the curren target
	target_texture()->bind(GL_TEXTURE_2D);
	target_rbo_.bind(GL_RENDERBUFFER);
	target_fbo_.bind(GL_DRAW_FRAMEBUFFER);

	target_fbo_.texture(GL_COLOR_ATTACHMENT0, *target_texture(), 0);

	// Render the contents of this buffer
	render_gl_contents(context);
}


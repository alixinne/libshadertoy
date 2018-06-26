#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/gl_buffer.hpp"
#include "shadertoy/render_context.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

gl_buffer::gl_buffer(const std::string &id)
	: basic_buffer(id)
{
}

void gl_buffer::init_contents(const render_context &context, const io_resource &io)
{
	// Nothing currently
}

void gl_buffer::allocate_contents(const render_context &context, const io_resource &io)
{
	// Setup render buffers
	target_rbo_.bind(GL_RENDERBUFFER);

	// Resolve size
	rsize size(io.render_size()->resolve());
	target_rbo_.storage(GL_DEPTH_COMPONENT, size.width, size.height);
}

void gl_buffer::render_contents(const render_context &context, const io_resource &io)
{
	// Update renderbuffer to use the correct target texture and bind as the curren target
	target_rbo_.bind(GL_RENDERBUFFER);
	target_fbo_.bind(GL_DRAW_FRAMEBUFFER);

	target_fbo_.texture(GL_COLOR_ATTACHMENT0, *io.target_texture(), 0);

	// Render the contents of this buffer
	render_gl_contents(context, io);
}


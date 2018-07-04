#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/gl_buffer.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

using shadertoy::gl::gl_call;
using shadertoy::utils::error_assert;

gl_buffer::gl_buffer(const std::string &id)
	: basic_buffer(id),
	clear_color_{0.f, 0.f, 0.f, 0.f},
	clear_depth_(0.f),
	clear_bits_(0)
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
	error_assert(size.width != 0 && size.height != 0,
				 "Render size for gl_buffer {} ({}) is zero",
				 id(),
				 (void*)this);

	target_rbo_.storage(GL_DEPTH_COMPONENT, size.width, size.height);
}

void gl_buffer::render_contents(const render_context &context, const io_resource &io)
{
	// Update renderbuffer to use the correct target texture and bind as the current target
	target_rbo_.bind(GL_RENDERBUFFER);

	if (io.swap_policy() == member_swap_policy::default_framebuffer)
	{
		// Bind default framebuffer, assume viewport has been set correctly
		gl_call(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);

		// Do not set the viewport, we are drawing to the default framebuffer so it is
		// configured by the user.
	}
	else
	{
		target_fbo_.bind(GL_DRAW_FRAMEBUFFER);

		// Set color attachements
		auto &texture(io.target_texture());
		error_assert(texture.get() != nullptr,
					 "Target render texture for gl_buffer {} ({}) was not allocated before rendering",
					 id(),
					 (void*)this);

		target_fbo_.texture(GL_COLOR_ATTACHMENT0, *texture, 0);

		// Set the viewport
		auto size(io.render_size()->resolve());
		gl_call(glViewport, 0, 0, size.width, size.height);
	}

	// Clear buffers as requested
	if (clear_bits_ & GL_COLOR_BUFFER_BIT)
		gl_call(glClearColor, clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]);

	if (clear_bits_ & GL_DEPTH_BUFFER_BIT)
		gl_call(glClearDepthf, clear_depth_);

	if (clear_bits_)
		gl_call(glClear, clear_bits_);

	// Render the contents of this buffer
	render_gl_contents(context, io);
}


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
	target_fbo_.framebuffer_renderbuffer(GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target_rbo_);

	// Resolve size
	rsize size(io.render_size()->resolve());
	error_assert(size.width != 0 && size.height != 0,
				 "Render size for gl_buffer {} ({}) is zero",
				 id(),
				 (void*)this);

	target_rbo_.storage(GL_DEPTH_COMPONENT, size.width, size.height);
}

void gl_buffer::render_contents(const render_context &context, const io_resource &io,
								const members::buffer_member &member)
{
	if (io.swap_policy() == member_swap_policy::default_framebuffer)
	{
		// Bind default framebuffer, assume viewport has been set correctly
		gl_call(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);

		// Do not set the viewport, we are drawing to the default framebuffer so it is
		// configured by the user.
	}
	else
	{
		auto fbo_bind(gl::get_bind_guard(target_fbo_, GL_DRAW_FRAMEBUFFER));

		// Set color attachements
		auto &texture(io.target_texture());
		error_assert(texture.get() != nullptr,
					 "Target render texture for gl_buffer {} ({}) was not allocated before rendering",
					 id(),
					 (void*)this);

		attach_framebuffer_outputs(fbo_bind, *texture);

		// Set the viewport
		auto size(io.render_size()->resolve());
		gl_call(glViewport, 0, 0, size.width, size.height);

		// Drop the bind guard to leave the framebuffer bound
		gl::drop_bind_guard(std::move(fbo_bind));
	}

	// Apply member state
	auto &state(member.state());
	state.apply();

	// Clear buffers as requested
	state.clear();

	// Render the contents of this buffer
	render_gl_contents(context, io);
}

void gl_buffer::attach_framebuffer_outputs(const gl::bound_ops<gl::framebuffer> &target_fbo,
										   const gl::texture &texture)
{
	target_fbo.resource().texture(GL_COLOR_ATTACHMENT0, texture, 0);
}

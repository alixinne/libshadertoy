#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

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

	// Assert we have at least one render target
	error_assert(!io.output_specs().empty(), "No render targets defined for gl_buffer {} ({})",
				 id(), static_cast<const void *>(this));

	// Resolve size of first target
	rsize size(io.output_specs().front().render_size->resolve());
	error_assert(size.width != 0 && size.height != 0, "Render size for gl_buffer {} ({}) first target is zero",
				 id(), static_cast<const void *>(this));

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
		attach_framebuffer_outputs(fbo_bind, io);

		// Set the viewport
		auto size(io.output_specs().front().render_size->resolve());
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

void gl_buffer::attach_framebuffer_outputs(const gl::bind_guard<gl::framebuffer, GLint> &target_fbo,
										   const io_resource &io)
{
	std::vector<GLenum> draw_buffers(std::get<1>(std::max_element(io.output_specs().begin(),
																  io.output_specs().end(),
																  [](const auto &a, const auto &b) {
																	  return std::get<1>(a.name) <
																			 std::get<1>(b.name);
																  })
												 ->name) +
									 1,
									 GL_NONE);

	for (size_t idx = 0; idx != io.output_specs().size(); ++idx)
	{
		auto &texture(io.target_texture(idx));
		error_assert(texture != nullptr, "Target render texture for gl_buffer {} ({}) was not allocated before rendering",
					 id(), static_cast<const void *>(this));

		target_fbo.texture_2d(std::get<0>(target_fbo.args()), GL_COLOR_ATTACHMENT0 + idx,
							  GL_TEXTURE_2D, *texture, 0);
		draw_buffers[std::get<1>(io.output_specs()[idx].name)] = GL_COLOR_ATTACHMENT0 + idx;
	}

	target_fbo.draw_buffers(draw_buffers.size(), draw_buffers.data());
}

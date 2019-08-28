#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/backends/gx/program.hpp"

#include "shadertoy/buffers/gl_buffer.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

using shadertoy::utils::error_assert;

gl_buffer::gl_buffer(const std::string &id)
: graphics_buffer(id), target_fbo_(backends::current()->make_framebuffer()),
  target_rbo_(backends::current()->make_renderbuffer())
{
}

void gl_buffer::init(const render_context &context, const io_resource &io)
{
	// Nothing currently
}

void gl_buffer::allocate_textures(const render_context &context, const io_resource &io)
{
	// Setup render buffers
	target_fbo_->framebuffer_renderbuffer(GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *target_rbo_);

	// Assert we have at least one render target
	error_assert(!io.output_specs().empty(), "No render targets defined for gl_buffer {} ({})",
				 id(), static_cast<const void *>(this));

	// Resolve size of first target
	rsize size(io.output_specs().front().render_size->resolve());
	error_assert(size.width != 0 && size.height != 0, "Render size for gl_buffer {} ({}) first target is zero",
				 id(), static_cast<const void *>(this));

	target_rbo_->storage(GL_DEPTH_COMPONENT16, size.width, size.height);
}

void gl_buffer::render(const render_context &context, const io_resource &io, const members::buffer_member &member)
{
	rsize size;

	if (io.swap_policy() == member_swap_policy::default_framebuffer)
	{
		// Bind default framebuffer, assume viewport has been set correctly
		backends::current()->bind_default_framebuffer(GL_DRAW_FRAMEBUFFER);

		// Do not set the viewport, we are drawing to the default framebuffer so it is
		// configured by the user.

		// Get default viewport target size
		GLint viewport[4]; // x, y, width, height
		backends::current()->get_viewport(viewport);
		size = rsize(viewport[2], viewport[3]);
	}
	else
	{
		target_fbo_->bind(GL_DRAW_FRAMEBUFFER);

		// Set the viewport
		size = io.output_specs().front().render_size->resolve();
		target_fbo_->set_viewport(0, 0, size.width, size.height);

		// Set color attachements
		attach_framebuffer_outputs(GL_DRAW_FRAMEBUFFER, *target_fbo_, io);
	}

	// Apply member state
	auto &state(member.state());
	state.apply();

	// Clear buffers as requested
	state.clear();

	// Render the contents of this buffer
	render_gl_contents(context, io, size);
}

void gl_buffer::attach_framebuffer_outputs(GLenum target, const backends::gx::framebuffer &target_fbo,
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

		target_fbo.texture_2d(target, GL_COLOR_ATTACHMENT0 + idx, GL_TEXTURE_2D, *texture, 0);
		draw_buffers[std::get<1>(io.output_specs()[idx].name)] = GL_COLOR_ATTACHMENT0 + idx;
	}

	target_fbo.draw_buffers(draw_buffers.size(), draw_buffers.data());
}

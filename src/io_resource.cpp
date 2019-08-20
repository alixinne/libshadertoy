#include <epoxy/gl.h>

#include "shadertoy/io_resource.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;

using shadertoy::utils::error_assert;
using shadertoy::utils::log;
using shadertoy::utils::warn_assert;

output_buffer_spec::output_buffer_spec(rsize_ref render_size, output_name_info_t name, GLint internal_format)
: render_size(std::move(render_size)), name(name), internal_format(internal_format)
{
}

void io_resource::output_buffer::allocate(const output_buffer_spec &spec, const io_resource *resource)
{
	member_swap_policy swap_policy(resource->swap_policy_);

	rsize size(spec.render_size->resolve());
	error_assert(size.width > 0 && size.height > 0, "IO resource object {} size is zero",
				 static_cast<const void *>(this));

	// Current texture settings
	rsize current_size;
	GLint current_format(0);
	member_swap_policy current_policy(member_swap_policy::default_framebuffer);

	// If the textures exist, read their parameters
	if (source_tex)
	{
		GLint width, height;
		source_tex->get_parameter(0, GL_TEXTURE_WIDTH, &width);
		source_tex->get_parameter(0, GL_TEXTURE_HEIGHT, &height);
		source_tex->get_parameter(0, GL_TEXTURE_INTERNAL_FORMAT, &current_format);
		current_size = rsize(width, height);

		current_policy = member_swap_policy::single_buffer;
	}

	// Check for target_tex_ to detect double buffering
	if (target_tex)
	{
		current_policy = member_swap_policy::double_buffer;
	}

	if (current_size != size || current_format != spec.internal_format || current_policy != swap_policy)
	{
		switch (swap_policy)
		{
		case member_swap_policy::double_buffer:
			init_render_texture(spec, size, source_tex);
			init_render_texture(spec, size, target_tex);
			break;
		case member_swap_policy::single_buffer:
			init_render_texture(spec, size, source_tex);
			target_tex.reset();
			break;
		case member_swap_policy::default_framebuffer:
			source_tex.reset();
			target_tex.reset();
			break;
		}
	}
}

void io_resource::output_buffer::swap(const output_buffer_spec &spec, const io_resource *resource)
{
	if (resource->swap_policy_ != member_swap_policy::default_framebuffer)
	{
		if (warn_assert(source_tex != nullptr, "Swapping unallocated IO resource object {}",
						static_cast<const void *>(resource)))
		{
			GLint width, height, current_format;
			source_tex->get_parameter(0, GL_TEXTURE_WIDTH, &width);
			source_tex->get_parameter(0, GL_TEXTURE_HEIGHT, &height);
			source_tex->get_parameter(0, GL_TEXTURE_INTERNAL_FORMAT, &current_format);

			rsize current_size(width, height);
			warn_assert(current_size == spec.render_size->resolve() && current_format == spec.internal_format,
						"IO resource object {} render size and allocated sizes and/or formats "
						"mismatch",
						static_cast<const void *>(resource));

			warn_assert((target_tex && resource->swap_policy_ == member_swap_policy::double_buffer) ||
						(!target_tex && resource->swap_policy_ == member_swap_policy::single_buffer),
						"IO resource object {} swap policy doesn't match the current state",
						static_cast<const void *>(resource));
		}

		if (target_tex)
		{
			std::swap(source_tex, target_tex);
		}
	}
	else
	{
		warn_assert(source_tex == nullptr && target_tex == nullptr,
					"IO resource object {} swap policy doesn't match the current state",
					static_cast<const void *>(resource));
	}
}

void io_resource::output_buffer::init_render_texture(const output_buffer_spec &spec, rsize size,
													 std::unique_ptr<gl::texture> &texptr)
{
	// Only create a texture object if it is necessary
	if (!texptr)
	{
		texptr = std::make_unique<gl::texture>(GL_TEXTURE_2D);
	}

	// Allocate texture storage according to width/height
	texptr->image_2d(GL_TEXTURE_2D, 0, spec.internal_format, size.width, size.height, 0, GL_BGRA,
					 GL_UNSIGNED_BYTE, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	uint8_t black[4] = { 0 };
	texptr->clear_tex_image(0, GL_BGRA, GL_UNSIGNED_BYTE, std::begin(black));

	log::shadertoy()->debug("Initialized {}x{} ({}) render texture at {} for {} (GL id {})", size.width,
							size.height, spec.internal_format, static_cast<const void *>(texptr.get()),
							static_cast<const void *>(this), GLuint(*texptr));
}

io_resource::io_resource(member_swap_policy swap_policy) : swap_policy_(swap_policy) {}

void io_resource::allocate()
{
	// Resize outputs to number of specifications
	outputs_.resize(output_specs_.size());

	auto it_outp(outputs_.begin());
	auto it_spec(output_specs_.cbegin());

	for (; it_spec != output_specs_.end(); ++it_outp, ++it_spec)
	{
		// Copy spec into existing
		it_outp->allocate(*it_spec, this);
	}
}

void io_resource::swap()
{
	auto it_outp(outputs_.begin());
	auto it_spec(output_specs_.cbegin());

	for (; it_spec != output_specs_.end(); ++it_outp, ++it_spec)
	{
		// Copy spec into existing
		it_outp->swap(*it_spec, this);
	}
}

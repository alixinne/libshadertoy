#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/io_resource.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;

using shadertoy::utils::log;
using shadertoy::utils::warn_assert;
using shadertoy::utils::error_assert;

void io_resource::init_render_texture(rsize size, std::unique_ptr<gl::texture> &texptr)
{
	// Only create a texture object if it is necessary
	if (!texptr)
	{
		texptr = std::make_unique<gl::texture>(GL_TEXTURE_2D);
	}

	// Allocate texture storage according to width/height
	texptr->image_2d(GL_TEXTURE_2D, 0, internal_format_, size.width, size.height, 0, GL_BGRA,
					 GL_UNSIGNED_BYTE, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	uint8_t black[4] = { 0 };
	texptr->clear_tex_image(0, GL_BGRA, GL_UNSIGNED_BYTE, std::begin(black));

	log::shadertoy()->debug("Initialized {}x{} ({}) render texture at {} for {} (GL id {})",
							size.width, size.height, internal_format_, static_cast<const void *>(texptr.get()),
							static_cast<const void *>(this), GLuint(*texptr));
}

io_resource::io_resource(rsize_ref &&render_size, GLint internal_format, member_swap_policy swap_policy)
	: render_size_(std::move(render_size)),
	internal_format_(internal_format),
	swap_policy_(swap_policy),
	source_tex_(),
	target_tex_()
{}

void io_resource::allocate()
{
	rsize size(render_size_->resolve());
	error_assert(size.width > 0 && size.height > 0, "IO resource object {} size is zero",
				 static_cast<const void *>(this));

	// Current texture settings
	rsize current_size;
	GLint current_format(0);
	member_swap_policy current_policy(member_swap_policy::default_framebuffer);

	// If the textures exist, read their parameters
	if (source_tex_)
	{
		GLint width, height;
		source_tex_->get_parameter(0, GL_TEXTURE_WIDTH, &width);
		source_tex_->get_parameter(0, GL_TEXTURE_HEIGHT, &height);
		source_tex_->get_parameter(0, GL_TEXTURE_INTERNAL_FORMAT, &current_format);
		current_size = rsize(width, height);

		current_policy = member_swap_policy::single_buffer;
	}

	// Check for target_tex_ to detect double buffering
	if (target_tex_)
	{
		current_policy = member_swap_policy::double_buffer;
	}

	if (current_size != size || current_format != internal_format_ || current_policy != swap_policy_)
	{
		switch (swap_policy_)
		{
			case member_swap_policy::double_buffer:
				init_render_texture(size, source_tex_);
				init_render_texture(size, target_tex_);
				break;
			case member_swap_policy::single_buffer:
				init_render_texture(size, source_tex_);
				target_tex_.reset();
				break;
			case member_swap_policy::default_framebuffer:
				source_tex_.reset();
				target_tex_.reset();
				break;
		}
	}
}

void io_resource::swap()
{
	if (swap_policy_ != member_swap_policy::default_framebuffer)
	{
		if (warn_assert(source_tex_ != nullptr, "Swapping unallocated IO resource object {}", static_cast<const void *>(this)))
		{
			GLint width, height, current_format;
			source_tex_->get_parameter(0, GL_TEXTURE_WIDTH, &width);
			source_tex_->get_parameter(0, GL_TEXTURE_HEIGHT, &height);
			source_tex_->get_parameter(0, GL_TEXTURE_INTERNAL_FORMAT, &current_format);

			rsize current_size(width, height);
			warn_assert(current_size == render_size_->resolve() && current_format == internal_format_,
						"IO resource object {} render size and allocated sizes and/or formats "
						"mismatch",
						static_cast<const void *>(this));

			warn_assert((target_tex_ && swap_policy_ == member_swap_policy::double_buffer) ||
						(!target_tex_ && swap_policy_ == member_swap_policy::single_buffer),
						"IO resource object {} swap policy doesn't match the current state",
						static_cast<const void *>(this));
		}

		if (target_tex_)
		{
			std::swap(source_tex_, target_tex_);
		}
	}
	else
	{
		warn_assert(source_tex_ == nullptr && target_tex_ == nullptr,
					"IO resource object {} swap policy doesn't match the current state", static_cast<const void *>(this));
	}
}

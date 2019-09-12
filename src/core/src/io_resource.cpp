#include "shadertoy/backends/gx/backend.hpp"

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
		source_tex->get_parameter(GL_TEXTURE_WIDTH, &width);
		source_tex->get_parameter(GL_TEXTURE_HEIGHT, &height);
		source_tex->get_parameter(GL_TEXTURE_INTERNAL_FORMAT, &current_format);
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
			source_tex->get_parameter(GL_TEXTURE_WIDTH, &width);
			source_tex->get_parameter(GL_TEXTURE_HEIGHT, &height);
			source_tex->get_parameter(GL_TEXTURE_INTERNAL_FORMAT, &current_format);

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

std::pair<GLenum, GLenum> get_preferred_format(GLenum internal_format)
{
	switch (internal_format)
	{
		case GL_R8:
			return {GL_RED, GL_UNSIGNED_BYTE};
		case GL_R8_SNORM:
			return {GL_RED, GL_BYTE};
		case GL_R16F:
			return {GL_RED, GL_HALF_FLOAT};
		case GL_R32F:
			return {GL_RED, GL_FLOAT};
		case GL_R8UI:
			return {GL_RED_INTEGER, GL_UNSIGNED_BYTE};
		case GL_R8I:
			return {GL_RED_INTEGER, GL_BYTE};
		case GL_R16UI:
			return {GL_RED_INTEGER, GL_UNSIGNED_SHORT};
		case GL_R16I:
			return {GL_RED_INTEGER, GL_SHORT};
		case GL_R32UI:
			return {GL_RED_INTEGER, GL_UNSIGNED_INT};
		case GL_R32I:
			return {GL_RED_INTEGER, GL_INT};

		case GL_RG8:
			return {GL_RG, GL_UNSIGNED_BYTE};
		case GL_RG8_SNORM:
			return {GL_RG, GL_BYTE};
		case GL_RG16F:
			return {GL_RG, GL_HALF_FLOAT};
		case GL_RG32F:
			return {GL_RG, GL_FLOAT};
		case GL_RG8UI:
			return {GL_RG_INTEGER, GL_UNSIGNED_BYTE};
		case GL_RG8I:
			return {GL_RG_INTEGER, GL_BYTE};
		case GL_RG16UI:
			return {GL_RG_INTEGER, GL_UNSIGNED_SHORT};
		case GL_RG16I:
			return {GL_RG_INTEGER, GL_SHORT};
		case GL_RG32UI:
			return {GL_RG_INTEGER, GL_UNSIGNED_INT};
		case GL_RG32I:
			return {GL_RG_INTEGER, GL_INT};

		case GL_RGB8:
		case GL_SRGB8:
		case GL_RGB565:
			return {GL_RGB, GL_UNSIGNED_BYTE};
		case GL_RGB8_SNORM:
			return {GL_RGB, GL_BYTE};
		case GL_R11F_G11F_B10F:
			return {GL_RGB, GL_FLOAT};
		case GL_RGB9_E5:
			return {GL_RGB, GL_FLOAT};
		case GL_RGB16F:
			return {GL_RGB, GL_HALF_FLOAT};
		case GL_RGB32F:
			return {GL_RGB, GL_FLOAT};
		case GL_RGB8UI:
			return {GL_RGB_INTEGER, GL_UNSIGNED_BYTE};
		case GL_RGB8I:
			return {GL_RGB_INTEGER, GL_BYTE};
		case GL_RGB16UI:
			return {GL_RGB_INTEGER, GL_UNSIGNED_SHORT};
		case GL_RGB16I:
			return {GL_RGB_INTEGER, GL_SHORT};
		case GL_RGB32UI:
			return {GL_RGB_INTEGER, GL_UNSIGNED_INT};
		case GL_RGB32I:
			return {GL_RGB_INTEGER, GL_INT};

		case GL_RGBA8:
			return {GL_RGBA, GL_UNSIGNED_BYTE};
		case GL_SRGB8_ALPHA8:
			return {GL_RGBA, GL_UNSIGNED_BYTE};
		case GL_RGBA8_SNORM:
			return {GL_RGBA, GL_BYTE};
		case GL_RGB5_A1:
			return {GL_RGBA, GL_UNSIGNED_BYTE};
		case GL_RGBA4:
			return {GL_RGBA, GL_UNSIGNED_BYTE};
		case GL_RGB10_A2:
			return {GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV};
		case GL_RGBA16F:
			return {GL_RGBA, GL_HALF_FLOAT};
		case GL_RGBA32F:
			return {GL_RGBA, GL_FLOAT};
		case GL_RGBA8UI:
			return {GL_RGBA_INTEGER, GL_UNSIGNED_BYTE};
		case GL_RGBA8I:
			return {GL_RGBA_INTEGER, GL_BYTE};
		case GL_RGB10_A2UI:
			return {GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV};
		case GL_RGBA16UI:
			return {GL_RGBA_INTEGER, GL_UNSIGNED_SHORT};
		case GL_RGBA16I:
			return {GL_RGBA_INTEGER, GL_SHORT};
		case GL_RGBA32UI:
			return {GL_RGBA_INTEGER, GL_UNSIGNED_INT};
		case GL_RGBA32I:
			return {GL_RGBA_INTEGER, GL_INT};

		case GL_DEPTH_COMPONENT16:
			return {GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT};
		case GL_DEPTH_COMPONENT24:
			return {GL_DEPTH_COMPONENT, GL_UNSIGNED_INT};
		case GL_DEPTH_COMPONENT32F:
			return {GL_DEPTH_COMPONENT, GL_FLOAT};

		case GL_DEPTH24_STENCIL8:
			return {GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};
		case GL_DEPTH32F_STENCIL8:
			return {GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV};

		default:
			return {internal_format, GL_UNSIGNED_BYTE};
	}
}

void io_resource::output_buffer::init_render_texture(const output_buffer_spec &spec, rsize size,
													 std::unique_ptr<backends::gx::texture> &texptr)
{
	// Only create a texture object if it is necessary
	if (!texptr)
	{
		texptr = backends::current()->make_texture(GL_TEXTURE_2D);
	}

	// Allocate texture storage according to width/height
	auto format = get_preferred_format(spec.internal_format);
	texptr->image_2d(GL_TEXTURE_2D, 0, spec.internal_format, size.width, size.height, 0, format.first,
					 format.second, nullptr);

#if SHADERTOY_HAS_CLEAR_TEX_IMAGE
	// Clear the target texture so it doesn't contain garbage
	uint8_t black[4] = { 0 };
	texptr->clear_tex_image(0, format.first, GL_UNSIGNED_BYTE, std::begin(black));
#endif

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

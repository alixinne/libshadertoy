#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/io_resource.hpp"

#include "shadertoy/utils/log.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;

void io_resource::init_render_texture(rsize size, std::shared_ptr<gl::texture> &texptr)
{
	// Only create a texture object if it is necessary
	if (!texptr)
		texptr = std::make_shared<gl::texture>(GL_TEXTURE_2D);

	// Allocate texture storage according to width/height
	texptr->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, size.width, size.height, 0, GL_BGRA,
					 GL_UNSIGNED_BYTE, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	float black[4] = {0.f};
	texptr->clear_tex_image(0, GL_BGRA, GL_FLOAT, black);
}

io_resource::io_resource(rsize_ref &&render_size)
	: render_size_(std::move(render_size)),
	current_size_(),
	source_tex_(),
	target_tex_()
{}

void io_resource::allocate()
{
	rsize size(render_size_->resolve());
	assert(size.width > 0);
	assert(size.height > 0);

	if (current_size_ != size)
	{
		init_render_texture(size, source_tex_);
		init_render_texture(size, target_tex_);

		current_size_ = size;
	}
}

void io_resource::swap()
{
	if (current_size_ != render_size_->resolve())
		log::shadertoy()->warn("IO resource object {} render size and allocated sizes mismatch", (void*)this);

	std::swap(source_tex_, target_tex_);
}

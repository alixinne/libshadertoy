#include <epoxy/gl.h>

#include "shadertoy/inputs/texture_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

GLenum texture_input::load_input() { return internal_format_; }

void texture_input::reset_input() {}

gl::texture *texture_input::use_input() { return image_texture_.get(); }

texture_input::texture_input(gl::texture image_texture, GLenum internal_format)
: image_texture_(std::make_unique<gl::texture>(std::move(image_texture))), internal_format_(internal_format)
{
}

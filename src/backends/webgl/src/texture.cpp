#include <GLES3/gl3.h>

#include "shadertoy/backends/webgl/texture.hpp"
#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/backends/webgl/backend.hpp"

using namespace shadertoy::backends;
using namespace shadertoy::backends::webgl;

null_texture_error::null_texture_error()
: shadertoy::shadertoy_error("An attempt was made to dereference a null texture")
{
}

texture::texture(GLenum target) : resource(), target_(target) {}

void texture::bind(GLenum target) const { target_ = target; backend::current().bind_texture(target, *this); }

void texture::unbind(GLenum target) const { backend::current().bind_texture(target, std::nullopt); }

void texture::bind_unit(GLuint unit) const { backend::current().bind_texture_unit(unit, *this); }

void texture::parameter(GLenum pname, GLint param) const
{
	bind(target_);
	gl_call(glTexParameteri, target_, pname, param);
}

void texture::parameter(GLenum pname, GLfloat param) const
{
	bind(target_);
	gl_call(glTexParameterf, target_, pname, param);
}

void texture::get_parameter(GLenum pname, GLint *params) const
{
	bind(target_);
	gl_call(glGetTexParameteriv, target_, pname, params);
}

void texture::get_parameter(GLenum pname, GLfloat *params) const
{
	bind(target_);
	gl_call(glGetTexParameterfv, target_, pname, params);
}

void texture::image_2d(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
					   GLint border, GLenum format, GLenum type, const GLvoid *data) const
{
	bind(target);
	gl_call(glTexImage2D, target, level, internalFormat, width, height, border, format, type, data);
}

void texture::generate_mipmap() const
{
	bind(target_);
	gl_call(glGenerateMipmap, target_);
}

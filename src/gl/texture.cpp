#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/texture.hpp"

using namespace shadertoy::gl;

null_texture_error::null_texture_error()
	: shadertoy_error("An attempt was made to dereference a null texture")
{
}

GLuint texture_allocator::create(GLenum target)
{
	GLuint res;
    gl_call(glCreateTextures, target, 1, &res);
	return res;
}

void texture_allocator::destroy(GLuint resource)
{
    gl_call(glDeleteTextures, 1, &resource);
}

texture::texture(GLenum target)
	: resource(allocator_type().create(target))
{
}

void texture::bind(GLenum target) const
{
    gl_call(glBindTexture, target, GLuint(*this));
}

void texture::unbind(GLenum target) const
{
	gl_call(glBindTexture, target, 0);
}

void texture::bind_unit(GLuint unit) const
{
	gl_call(glBindTextureUnit, unit, GLuint(*this));
}

void texture::parameter(GLenum pname, GLint param) const
{
    gl_call(glTextureParameteri, GLuint(*this), pname, param);
}

void texture::parameter(GLenum pname, GLfloat param) const
{
    gl_call(glTextureParameterf, GLuint(*this), pname, param);
}

void texture::get_parameter(GLint level, GLenum pname, GLint *params) const
{
	gl_call(glGetTextureLevelParameteriv, GLuint(*this), level, pname, params);
}

void texture::get_parameter(GLint level, GLenum pname, GLfloat *params) const
{
	gl_call(glGetTextureLevelParameterfv, GLuint(*this), level, pname, params);
}

void texture::image_2d(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
					   GLint border, GLenum format, GLenum type, const GLvoid *data) const
{
    gl_call(glTextureImage2DEXT, GLuint(*this), target, level, internalFormat, width, height, border, format, type,
            data);
}

void texture::generate_mipmap() const
{
    gl_call(glGenerateTextureMipmap, GLuint(*this));
}

void texture::clear_tex_image(GLint level, GLenum format, GLenum type, const void *data) const
{
	gl_call(glClearTexImage, GLuint(*this), level, format, type, data);
}

void texture::get_image(GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) const
{
	gl_call(glGetTextureImage, GLuint(*this), level, format, type, bufSize, pixels);
}

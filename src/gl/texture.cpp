#include <epoxy/gl.h>

#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/gl/texture.hpp"

using namespace shadertoy::gl;

null_texture_error::null_texture_error()
	: ShadertoyError("An attempt was made to dereference a null texture")
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

void texture::bind(GLenum target)
{
    gl_call(glBindTexture, target, GLuint(*this));
}

void texture::parameter(GLenum pname, GLint param)
{
    gl_call(glTextureParameteri, GLuint(*this), pname, param);
}

void texture::parameter(GLenum pname, GLfloat param)
{
    gl_call(glTextureParameterf, GLuint(*this), pname, param);
}

void texture::image_2d(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border,
                       GLenum format, GLenum type, const GLvoid *data)
{
    gl_call(glTextureImage2DEXT, GLuint(*this), target, level, internalFormat, width, height, border, format, type,
            data);
}

void texture::generate_mipmap()
{
    gl_call(glGenerateTextureMipmap, GLuint(*this));
}

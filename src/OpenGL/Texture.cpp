#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Texture.hpp"

using namespace shadertoy::OpenGL;

NullTextureError::NullTextureError()
	: ShadertoyError("An attempt was made to dereference a null texture")
{
}

GLuint TextureAllocator::Create(GLenum target)
{
	GLuint res;
	glCall(glGenTextures, 1, &res);
	return res;
}

void TextureAllocator::Delete(GLuint resource)
{
	glCall(glDeleteTextures, 1, &resource);
}

Texture::Texture(GLenum target)
	: Resource(Allocator().Create(target))
{
}

void Texture::Bind(GLenum target)
{
	glCall(glBindTexture, target, GLuint(*this));
}

void Texture::Parameter(GLenum target, GLenum pname, GLint param)
{
	glCall(glTextureParameteriEXT, GLuint(*this), target, pname, param);
}

void Texture::Parameter(GLenum target, GLenum pname, GLfloat param)
{
	glCall(glTextureParameterfEXT, GLuint(*this), target, pname, param);
}

void Texture::Image2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data)
{
	glCall(glTextureImage2DEXT, GLuint(*this), target, level, internalFormat, width, height, border, format, type, data);
}

void Texture::GenerateMipmap()
{
	glCall(glGenerateMipmap, GLuint(*this));
}

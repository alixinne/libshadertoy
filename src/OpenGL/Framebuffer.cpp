#include <epoxy/gl.h>

#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Texture.hpp"
#include "shadertoy/OpenGL/Framebuffer.hpp"

using namespace shadertoy::OpenGL;

NullFramebufferError::NullFramebufferError()
	: ShadertoyError("An attempt was made to dereference a null framebuffer")
{
}

void Framebuffer::Bind(GLenum target)
{
	glCall(glBindFramebuffer, target, GLuint(*this));
}

void Framebuffer::Texture(GLenum attachment, shadertoy::OpenGL::Texture &texture, GLint level)
{
	glCall(glNamedFramebufferTexture, GLuint(*this), attachment, GLuint(texture), level);
}

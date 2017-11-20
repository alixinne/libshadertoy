#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Texture.hpp"

using namespace shadertoy::OpenGL;

NullTextureError::NullTextureError()
	: ShadertoyError("An attempt was made to dereference a null texture")
{
}

void Texture::Bind(GLenum target)
{
	glCall(glBindTexture, target, GLuint(*this));
}

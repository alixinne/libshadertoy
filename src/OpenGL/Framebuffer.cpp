#include "stdafx.hpp"
#include "shadertoy/OpenGL/Framebuffer.cpp"

using namespace shadertoy::OpenGL;

NullFramebufferError::NullFramebufferError()
	: ShadertoyError("An attempt was made to dereference a null framebuffer")
{
}

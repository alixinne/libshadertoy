#include "stdafx.hpp"
#include "shadertoy/OpenGL/Renderbuffer.hpp"

using namespace shadertoy::OpenGL;

NullRenderbufferError::NullRenderbufferError()
	: ShadertoyError("An attempt was made to dereference a null renderbuffer")
{
}

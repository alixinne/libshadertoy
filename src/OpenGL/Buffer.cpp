#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Buffer.hpp"

using namespace shadertoy::OpenGL;

NullBufferError::NullBufferError()
	: ShadertoyError("An attempt was made to dereference a null buffer")
{
}

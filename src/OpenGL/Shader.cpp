#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Shader.hpp"

using namespace shadertoy::OpenGL;

NullShaderError::NullShaderError()
	: ShadertoyError("An attempt was made to dereference a null shader")
{
}

#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Program.hpp"

using namespace shadertoy::OpenGL;

NullProgramError::NullProgramError()
	: ShadertoyError("An attempt was made to dereference a null program")
{
}

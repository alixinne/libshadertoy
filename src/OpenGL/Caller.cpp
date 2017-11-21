#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Caller.hpp"

#define ERROR_PREFIX "OpenGL error: "

using namespace shadertoy::OpenGL;

std::string glErrorToString(GLenum error)
{
	switch (error)
	{
		case GL_NO_ERROR:
			return ERROR_PREFIX "No error.";
		case GL_INVALID_ENUM:
			return ERROR_PREFIX "An unacceptable value is specified for an enumerated argument.";
		case GL_INVALID_VALUE:
			return ERROR_PREFIX "A numeric argument is out of range.";
		case GL_INVALID_OPERATION:
			return ERROR_PREFIX "The specified operation is not allowed in the current state.";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return ERROR_PREFIX "The framebuffer object is not complete.";
		case GL_OUT_OF_MEMORY:
			return ERROR_PREFIX "There is not enough memory left to execute the command.";
		case GL_STACK_UNDERFLOW:
			return ERROR_PREFIX "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
		case GL_STACK_OVERFLOW:
			return ERROR_PREFIX "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
	}

	std::stringstream ss;
	ss << ERROR_PREFIX "Unknown error (" << (int)error << ")";
	return ss.str();
}

OpenGLError::OpenGLError(GLenum error)
	: ShadertoyError(glErrorToString(error))
{
}

namespace shadertoy
{
namespace OpenGL
{
	void CheckErrors()
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			throw OpenGLError(error);
		}
	}
}
}

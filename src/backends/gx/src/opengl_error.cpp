#include <sstream>

#include "shadertoy/backends/gx/opengl_error.hpp"

#define ERROR_PREFIX "OpenGL error: "

using namespace shadertoy::backends::gx;

std::string gl_error_to_string(GLenum error, const std::string &extraMsg)
{
	std::stringstream ss;
	ss << ERROR_PREFIX;

	switch (error)
	{
	case GL_NO_ERROR:
		ss << "GL_NO_ERROR: No error.";
		break;
	case GL_INVALID_ENUM:
		ss << "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
		break;
	case GL_INVALID_VALUE:
		ss << "GL_INVALID_VALUE: A numeric argument is out of range.";
		break;
	case GL_INVALID_OPERATION:
		ss << "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		ss << "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
		break;
	case GL_OUT_OF_MEMORY:
		ss << "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
		break;
	case GL_STACK_UNDERFLOW:
		ss << "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would "
			  "cause an internal stack to underflow.";
		break;
	case GL_STACK_OVERFLOW:
		ss << "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would "
			  "cause an internal stack to overflow.";
		break;
	default:
		ss << "Unknown error (" << static_cast<int>(error) << ")";
		break;
	}

	if (!extraMsg.empty())
	{
		ss << std::endl << extraMsg;
	}

	return ss.str();
}

opengl_error::opengl_error(GLenum error, const std::string &extraMsg)
: shadertoy_error(gl_error_to_string(error, extraMsg))
{
}

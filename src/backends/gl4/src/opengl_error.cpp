#include <cstring>
#include <sstream>

#include <epoxy/gl.h>

#include "shadertoy/backends/gl4/opengl_error.hpp"

#define ERROR_PREFIX "OpenGL error: "

using namespace shadertoy::backends::gl4;

std::string gl_error_to_string(GLenum error, const std::string &extraMsg)
{
	std::stringstream ss;
	ss << ERROR_PREFIX;

	switch (error)
	{
		case GL_NO_ERROR:
			ss << "No error.";
			break;
		case GL_INVALID_ENUM:
			ss << "An unacceptable value is specified for an enumerated argument.";
			break;
		case GL_INVALID_VALUE:
			ss << "A numeric argument is out of range.";
			break;
		case GL_INVALID_OPERATION:
			ss << "The specified operation is not allowed in the current state.";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			ss << "The framebuffer object is not complete.";
			break;
		case GL_OUT_OF_MEMORY:
			ss << "There is not enough memory left to execute the command.";
			break;
		case GL_STACK_UNDERFLOW:
			ss << "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			ss << "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
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

#include "stdafx.hpp"
#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL/Caller.hpp"

#if HAS_UNWIND
#include <libunwind.h>
#endif

#define ERROR_PREFIX "OpenGL error: "

using namespace shadertoy::OpenGL;

std::string glErrorToString(GLenum error, const std::string &extraMsg)
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
			ss << "Unknown error (" << (int)error << ")";
			break;
	}

	if (!extraMsg.empty())
	{
		ss << std::endl << extraMsg;
	}

	return ss.str();
}

OpenGLError::OpenGLError(GLenum error, const std::string &extraMsg)
	: ShadertoyError(glErrorToString(error, extraMsg))
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
#if HAS_UNWIND
			std::stringstream ss;
			bool first = true;
			char buf[256];

			unw_cursor_t c;
			unw_context_t ctxt;
			unw_word_t ip, sp, off;
			int ret;

			ret = unw_getcontext(&ctxt);
			if (ret)
			{
				// failed to get local context
				goto unwind_end;
			}

			ret = unw_init_local(&c, &ctxt);
			if (ret)
			{
				// failed to get cursor on context
				goto unwind_end;
			}

			while (unw_step(&c) > 0)
			{
				unw_get_reg(&c, UNW_REG_IP, &ip);
				unw_get_reg(&c, UNW_REG_SP, &sp);

				ret = unw_get_proc_name(&c, &buf[0], sizeof(buf), &off);
				if (first)
				{
					first = false;
				}
				else
				{
					if (ret != 0 && ret != UNW_ENOMEM)
					{
						ss << "Called from ?:" << std::hex << ip << ":?" << std::endl;
					}
					else
					{
						ss << "Called from " << buf << ":" << std::hex << ip << ":" << std::hex << off << std::endl;
					}
				}

				if (strcmp("main", buf) == 0)
					break;
			}
unwind_end:
			throw OpenGLError(error, ss.str());
#else
			throw OpenGLError(error, string());
#endif
		}
	}
}
}

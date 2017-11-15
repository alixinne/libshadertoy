#ifndef _SHADERTOY_OPENGL_PROGRAM_HPP_
#define _SHADERTOY_OPENGL_PROGRAM_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class NullProgramError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullProgramError();
	};

	class Program : public Resource<Program, SingleDeleter<&glDeleteProgram>, NullProgramError>
	{
	};
}
}

#endif /* _SHADERTOY_OPENGL_PROGRAM_HPP_ */

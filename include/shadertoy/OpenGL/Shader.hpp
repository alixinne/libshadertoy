#ifndef _SHADERTOY_OPENGL_SHADER_HPP_
#define _SHADERTOY_OPENGL_SHADER_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class NullShaderError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullShaderError();
	};

	class Shader : public Resource<Shader, SingleDeleter<&glDeleteShader>, NullShaderError>
	{
	};
}
}

#endif /* _SHADERTOY_OPENGL_SHADER_HPP_ */

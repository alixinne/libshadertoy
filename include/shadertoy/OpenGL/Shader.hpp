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

	class ShaderCompilationError : public shadertoy::ShadertoyError
	{
	public:
		explicit ShaderCompilationError(GLuint shaderId, const std::string &log);

		GLuint shaderId() const
		{ return _shaderId; }

		const std::string &log() const
		{ return _log; }

	private:
		const GLuint _shaderId;
		const std::string _log;
	};

	/**
	 * Implementation of the allocator pattern for shaders.
	 * Takes a shader type as a supplementary parameter compared to the standard
	 * resource allocators.
	 */
	class ShaderAllocator
	{
	public:
		GLuint Create(GLenum shaderType);
		void Delete(GLuint resource);
	};

	class Shader : public Resource<Shader, ShaderAllocator, NullShaderError>
	{
	public:
		/**
		 * Creates a new shader of the given type.
		 *
		 * @param shaderType Type of the new shader
		 */
		Shader(GLenum shaderType);

		// glShaderSource
		void Source(const std::string &string);
		void Source(const std::vector<std::string> &string);

		// glCompileShader
		void Compile();

		// glGetShaderInfoLog
		std::string Log();
	};
}
}

#endif /* _SHADERTOY_OPENGL_SHADER_HPP_ */

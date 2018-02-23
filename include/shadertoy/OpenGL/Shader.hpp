#ifndef _SHADERTOY_OPENGL_SHADER_HPP_
#define _SHADERTOY_OPENGL_SHADER_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null shader object.
	 */
	class shadertoy_EXPORT NullShaderError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the NullShaderError class.
		 */
		explicit NullShaderError();
	};

	/**
	 * @brief Error thrown when the compilation of a shader fails.
	 */
	class shadertoy_EXPORT ShaderCompilationError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the ShaderCompilationError class.
		 *
		 * @param  shaderId Id of the shader that triggered the error
		 * @param  log      Contents of the shader compilation log
		 */
		explicit ShaderCompilationError(GLuint shaderId, const std::string &log);

		/**
		 * @brief Gets the id of the shader that triggered the error.
		 *
		 * @return Id of the shader that triggered the error
		 */
		GLuint shaderId() const
		{ return _shaderId; }

		/**
		 * @brief Gets the compilation log for this error.
		 *
		 * @return Compilation log as a string.
		 */
		const std::string &log() const
		{ return _log; }

	private:
		/// Shader id
		const GLuint _shaderId;
		/// Compilation log
		const std::string _log;
	};

	/**
	 * @brief Implementation of the allocator pattern for shaders.
	 * Takes a shader type as a supplementary parameter compared to the standard
	 * resource allocators.
	 */
	class shadertoy_EXPORT ShaderAllocator
	{
	public:
		/**
		 * @brief Create a shader object of the given type.
		 * @param  shaderType Type of the shader to create
		 * @return            Created shader id
		 *
		 * @throws OpenGLError
		 */
		GLuint Create(GLenum shaderType);
		/**
		 * @brief Deletes the given shader.
		 * @param resource Id of the shader to delete.
		 *
		 * @throws OpenGLError
		 */
		void Delete(GLuint resource);
	};

	/**
	 * @brief Represents an OpenGL shader.
	 */
	class shadertoy_EXPORT Shader : public Resource<Shader, ShaderAllocator, NullShaderError>
	{
	public:
		/**
		 * @brief Creates a new shader of the given type.
		 *
		 * @param shaderType Type of the new shader
		 *
		 * @throws OpenGLError
		 */
		Shader(GLenum shaderType);

		/**
		 * @brief glShaderSource
		 *
		 * @param string Single source as a string
		 *
		 * @throws OpenGLError
		 * @throws NullShaderError
		 */
		void Source(const std::string &string);
		/**
		 * @brief glShaderSource
		 *
		 * @param string List of sources to add to the shader
		 *
		 * @throws OpenGLError
		 * @throws NullShaderError
		 */
		void Source(const std::vector<std::string> &string);

		/**
		 * @brief glCompileShader
		 *
		 * @throws OpenGLError
		 * @throws ShaderCompilationError
		 * @throws NullShaderError
		 */
		void Compile();

		/**
		 * @brief glGetShaderInfoLog
		 *
		 * @return Shader info log as a string
		 *
		 * @throws OpenGLError
		 * @throws NullShaderError
		 */
		std::string Log();
	};
}
}

#endif /* _SHADERTOY_OPENGL_SHADER_HPP_ */

#ifndef _SHADERTOY_GL_SHADER_HPP_
#define _SHADERTOY_GL_SHADER_HPP_

#include "shadertoy/gl/resource.hpp"

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null shader object.
	 */
	class shadertoy_EXPORT null_shader_error : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the NullShaderError class.
		 */
		explicit null_shader_error();
	};

	/**
	 * @brief Error thrown when the compilation of a shader fails.
	 */
	class shadertoy_EXPORT shader_compilation_error : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the ShaderCompilationError class.
		 *
		 * @param  shaderId Id of the shader that triggered the error
		 * @param  log      Contents of the shader compilation log
		 */
		explicit shader_compilation_error(GLuint shaderId, const std::string &log);

		/**
		 * @brief Gets the id of the shader that triggered the error.
		 *
		 * @return Id of the shader that triggered the error
		 */
		GLuint shader_id() const
		{ return shader_id_; }

		/**
		 * @brief Gets the compilation log for this error.
		 *
		 * @return Compilation log as a string.
		 */
		const std::string &log() const
		{ return log_; }

	private:
		/// Shader id
		const GLuint shader_id_;
		/// Compilation log
		const std::string log_;
	};

	/**
	 * @brief Implementation of the allocator pattern for shaders.
	 * Takes a shader type as a supplementary parameter compared to the standard
	 * resource allocators.
	 */
	class shadertoy_EXPORT shader_allocator
	{
	public:
		/**
		 * @brief Create a shader object of the given type.
		 * @param  shaderType Type of the shader to create
		 * @return            Created shader id
		 *
		 * @throws OpenGLError
		 */
		GLuint create(GLenum shaderType);

		/**
		 * @brief Deletes the given shader.
		 * @param resource Id of the shader to delete.
		 *
		 * @throws OpenGLError
		 */
		void destroy(GLuint resource);
	};

	/**
	 * @brief Represents an OpenGL shader.
	 */
	class shadertoy_EXPORT shader : public resource<shader, shader_allocator, null_shader_error>
	{
	public:
		/**
		 * @brief Creates a new shader of the given type.
		 *
		 * @param shaderType Type of the new shader
		 *
		 * @throws OpenGLError
		 */
		shader(GLenum shaderType);

		/**
		 * @brief glShaderSource
		 *
		 * @param string Single source as a string
		 *
		 * @throws OpenGLError
		 * @throws NullShaderError
		 */
		void source(const std::string &string);

		/**
		 * @brief glShaderSource
		 *
		 * @param string List of sources to add to the shader
		 *
		 * @throws OpenGLError
		 * @throws NullShaderError
		 */
		void source(const std::vector<std::string> &string);

		/**
		 * @brief glCompileShader
		 *
		 * @throws OpenGLError
		 * @throws ShaderCompilationError
		 * @throws NullShaderError
		 */
		void compile();

		/**
		 * @brief glGetShaderInfoLog
		 *
		 * @return Shader info log as a string
		 *
		 * @throws OpenGLError
		 * @throws NullShaderError
		 */
		std::string log();
	};
}
}

#endif /* _SHADERTOY_GL_SHADER_HPP_ */

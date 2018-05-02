#ifndef _SHADERTOY_GL_SHADER_HPP_
#define _SHADERTOY_GL_SHADER_HPP_

#include "shadertoy/gl/resource.hpp"

#include <string>
#include <vector>

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null shader object.
	 */
	class shadertoy_EXPORT null_shader_error : public shadertoy::shadertoy_error
	{
	public:
		/**
		 * @brief Initialize a new instance of the null_shader_error class.
		 */
		explicit null_shader_error();
	};

	/**
	 * @brief Error thrown when the compilation of a shader fails.
	 */
	class shadertoy_EXPORT shader_compilation_error : public shadertoy::shadertoy_error
	{
	public:
		/**
		 * @brief Initialize a new instance of the ShaderCompilationError class.
		 *
		 * @param  shaderId Id of the shader that triggered the error
		 * @param  log      Contents of the shader compilation log
		 */
		explicit shader_compilation_error(GLuint shaderId, const std::string &log);

		/**
		 * @brief Get the id of the shader that triggered the error.
		 *
		 * @return Id of the shader that triggered the error
		 */
		GLuint shader_id() const
		{ return shader_id_; }

		/**
		 * @brief Get the compilation log for this error.
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
		 * @throws opengl_error
		 */
		GLuint create(GLenum shaderType);

		/**
		 * @brief Delete the given shader.
		 * @param resource Id of the shader to delete.
		 *
		 * @throws opengl_error
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
		 * @brief Create a new shader of the given type.
		 *
		 * @param shaderType Type of the new shader
		 *
		 * @throws opengl_error
		 */
		shader(GLenum shaderType);

		/**
		 * @brief glShaderSource
		 *
		 * @param string Single source as a string
		 *
		 * @throws opengl_error
		 * @throws null_shader_error
		 */
		void source(const std::string &string) const;

		/**
		 * @brief glShaderSource
		 *
		 * @param string List of sources to add to the shader
		 *
		 * @throws opengl_error
		 * @throws null_shader_error
		 */
		void source(const std::vector<std::string> &string) const;

		/**
		 * @brief glCompileShader
		 *
		 * @throws opengl_error
		 * @throws ShaderCompilationError
		 * @throws null_shader_error
		 */
		void compile() const;

		/**
		 * @brief glGetShaderInfoLog
		 *
		 * @return Shader info log as a string
		 *
		 * @throws opengl_error
		 * @throws null_shader_error
		 */
		std::string log() const;
	};
}
}

#endif /* _SHADERTOY_GL_SHADER_HPP_ */

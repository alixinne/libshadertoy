#ifndef _SHADERTOY_BACKENDS_GX_SHADER_HPP_
#define _SHADERTOY_BACKENDS_GX_SHADER_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

#include "shadertoy/shadertoy_error.hpp"

#include <string>
#include <vector>

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
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
	explicit shader_compilation_error(GLuint shaderId, std::string log);

	/**
	 * @brief Get the id of the shader that triggered the error.
	 *
	 * @return Id of the shader that triggered the error
	 */
	GLuint shader_id() const { return shader_id_; }

	/**
	 * @brief Get the compilation log for this error.
	 *
	 * @return Compilation log as a string.
	 */
	const std::string &log() const { return log_; }

	private:
	/// Shader id
	const GLuint shader_id_;
	/// Compilation log
	const std::string log_;
};

/**
 * @brief Represents an OpenGL shader.
 */
class shadertoy_EXPORT shader : public resource
{
	public:
	/**
	 * @brief glShaderSource
	 *
	 * @param string Single source as a string
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	virtual void source(const std::string &string) const = 0;

	/**
	 * @brief glShaderSource
	 *
	 * @param string List of sources to add to the shader
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	virtual void source(const std::vector<std::string> &string) const = 0;

	/**
	 * @brief glCompileShader
	 *
	 * @throws opengl_error
	 * @throws ShaderCompilationError
	 * @throws null_shader_error
	 */
	virtual void compile() const = 0;

	/**
	 * @brief glGetShaderInfoLog
	 *
	 * @return Shader info log as a string
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	virtual std::string log() const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_SHADER_HPP_ */

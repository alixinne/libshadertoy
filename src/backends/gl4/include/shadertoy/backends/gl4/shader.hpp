#ifndef _SHADERTOY_BACKENDS_GL4_SHADER_HPP_
#define _SHADERTOY_BACKENDS_GL4_SHADER_HPP_

#include "shadertoy/backends/gl4/pre.hpp"
#include "shadertoy/backends/gl4/resource.hpp"
#include "shadertoy/backends/gx/shader.hpp"

#include "shadertoy/shadertoy_error.hpp"

#include <string>
#include <vector>

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an attempt is made to dereference a null shader object.
 */
class stbackend_gl4_EXPORT null_shader_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the null_shader_error class.
	 */
	explicit null_shader_error();
};

/**
 * @brief Implementation of the allocator pattern for shaders.
 * Takes a shader type as a supplementary parameter compared to the standard
 * resource allocators.
 */
class stbackend_gl4_EXPORT shader_allocator
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
class stbackend_gl4_EXPORT shader : public resource<shader, shader_allocator, null_shader_error, gx::shader>
{
	public:
	shader(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

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
	void source(const std::string &string) const override;

	/**
	 * @brief glShaderSource
	 *
	 * @param string List of sources to add to the shader
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	void source(const std::vector<std::string> &string) const override;

	/**
	 * @brief glCompileShader
	 *
	 * @throws opengl_error
	 * @throws ShaderCompilationError
	 * @throws null_shader_error
	 */
	void compile() const override;

	/**
	 * @brief glGetShaderInfoLog
	 *
	 * @return Shader info log as a string
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	std::string log() const override;
};
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_SHADER_HPP_ */

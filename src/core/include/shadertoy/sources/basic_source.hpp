#ifndef _SHADERTOY_SOURCES_BASIC_SOURCE_HPP_
#define _SHADERTOY_SOURCES_BASIC_SOURCE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/backends/gx/program.hpp"
#include "shadertoy/backends/gx/shader.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace shadertoy
{
namespace sources
{
/**
 * @brief Represents the source of a program
 */
class shadertoy_EXPORT basic_source
{
	public:
	virtual ~basic_source() = default;

	/**
	 * @brief Get the source for a given shader type
	 *
	 * @param shader_type Type of the shader to get the source for. Can be
	 *                    either GL_VERTEX_SHADER, GL_FRAGMENT_SHADER or
	 *                    GL_COMPUTE_SHADER, etc.
	 *
	 * @return Ordered list of (name, source) pairs
	 */
	virtual std::vector<std::pair<std::string, std::string>> get_source(GLenum shader_type) const = 0;

	/**
	 * @brief Get a precompiled shader object, if available
	 *
	 * @param shader_type Type of the shader to search for
	 *
	 * @return Reference to the precompiled shader object, or null if no such object is available
	 */
	virtual std::shared_ptr<backends::gx::shader> get_precompiled(GLenum shader_type) const;

	/**
	 * @brief Checks if this source has data for a given shader type
	 *
	 * @param shader_type Type of the shader to check.
	 *
	 * @return true if this source can provide the shader code
	 */
	virtual bool has_source(GLenum shader_type) const = 0;

	/**
	 * @brief Get a compiled shader object for the given type
	 *
	 * If a precompiled shader object is available, it will be used
	 * instead. Caching of such objects is handled by the descendant
	 * classes.
	 *
	 * @param shader_type Type of the shader to compile
	 *
	 * @return Compiled shader object
	 */
	std::shared_ptr<backends::gx::shader> compile_shader(GLenum shader_type) const;

	/**
	 * @brief Compile a set of shaders into a program object
	 *
	 * @param shader_types Shader types to attach to the program
	 *
	 * @return Compiled program object
	 */
	std::unique_ptr<backends::gx::program> compile_program(const std::vector<GLenum> &shader_types) const;
};
} // namespace sources
} // namespace shadertoy

#endif /* _SHADERTOY_SOURCES_BASIC_SOURCE_HPP_ */

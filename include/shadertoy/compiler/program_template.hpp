#ifndef _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_
#define _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/shader_template.hpp"

#include <map>

namespace shadertoy
{
namespace compiler
{

/**
 * @brief Represents the template for building programs in a chain.
 *
 * A program template defines the different parts of a program as
 * shader_template instances. Swap chain members can then derive those templates
 * to build suitable GPU programs for rendering.
 */
class shadertoy_EXPORT program_template
{
	/**
	 * @brief List of shader templates for this program template.
	 *
	 * We limit the number of each shader type to one, which prevents link-time
	 * use. However, users will usually use the shader_template class to combine
	 * sources.
	 */
	std::map<GLenum, shader_template> shader_templates_;

	/**
	 * @brief List of compiled shaders. Fully specified templates can be compiled to this
	 * cache, which will be reused when compiling derived programs.
	 */
	std::map<GLenum, gl::shader> compiled_shaders_;

public:
	/**
	 * @brief Initialize a new empty program_template
	 */
	program_template();

	/**
	 * @brief Initialize a new program_template with the specified shader
	 * templates.
	 *
	 * @param shader_templates Map of shader types to the corresponding templates
	 */
	program_template(std::map<GLenum, shader_template> shader_templates);

	/**
	 * @brief Add new shader_template in place
	 *
	 * @param type            Type of the shader template
	 * @param shader_template Shader template to add
	 */
	void emplace_back(GLenum type, shader_template &&shader_template);

	/**
	 * @brief Get a template of the given type.
	 *
	 * @param type Shader template type
	 *
	 * @return Reference to the shader_template object for the given shader type
	 */
	inline shader_template &operator[](GLenum type) { return shader_templates_.at(type); }

	/**
	 * @brief Get a template of the given type.
	 *
	 * @param type Shader template type
	 *
	 * @return Reference to the shader_template object for the given shader type
	 */
	inline const shader_template &operator[](GLenum type) const { return shader_templates_.at(type); }

	/**
	 * @brief Compiles a fully specified shader in the cache of this program_template
	 *
	 * @param type Type of the shader to precompile
	 */
	void compile(GLenum type);

	/**
	 * @brief Compile this program_template into a GL program.
	 *
	 * @param specifications Map of specifications for each shader template. This is
	 *                       used to specify missing parts in all templates before
	 *                       compiling. Pre-compiled shaders are used as-is and cannot
	 *                       be overwritten.
	 *
	 * @return Compiled program
	 */
	gl::program compile(std::map<GLenum, std::vector<std::unique_ptr<basic_part>>> parts) const;

	/**
	 * @brief Compile the given fully specified templates into a GL program.
	 *
	 * @param templates Map of fully specified templates. Entries from this map will
	 *                  be used instead of the precompiled shaders if there is a conflict.
	 *
	 * @return Compiled program
	 */
	gl::program compile(const std::map<GLenum, shader_template> &templates) const;
};
}
}

#endif /* _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_ */
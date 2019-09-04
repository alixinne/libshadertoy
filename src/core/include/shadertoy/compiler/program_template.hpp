#ifndef _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_
#define _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/define_part.hpp"
#include "shadertoy/compiler/shader_template.hpp"

#include "shadertoy/backends/gx/program.hpp"

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
	std::map<GLenum, std::shared_ptr<backends::gx::shader>> compiled_shaders_;

	/**
	 * @brief List of preprocessor definition objects
	 */
	std::map<std::string, std::shared_ptr<preprocessor_defines>> shader_defines_;

	shader_template specify_template_parts(const std::vector<std::unique_ptr<basic_part>> &parts,
										   const shader_template &source_template) const;

	std::vector<std::pair<std::string, std::string>>
	get_source(const shader_template &shader_template, const std::vector<std::unique_ptr<basic_part>> &parts) const;

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

	program_template(const program_template &) = delete;
	program_template &operator=(const program_template &) = delete;

	/**
	 * @brief Add new shader_template in place
	 *
	 * @param type            Type of the shader template
	 * @param shader_template Shader template to add
	 */
	bool emplace(GLenum type, shader_template &&shader_template);

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
	inline const shader_template &operator[](GLenum type) const
	{
		return shader_templates_.at(type);
	}

	/**
	 * @brief Checks if this template has the given shader type template
	 *
	 * @param type Shader type to check
	 *
	 * @return true if this template contains a shader template for the given type
	 */
	inline bool has_shader(GLenum type) const
	{
		return shader_templates_.find(type) != shader_templates_.end();
	}

	/**
	 * @brief Builds the source of a given shader for the given type
	 *
	 * @param type  Shader type to build
	 * @param parts Parts to fill in the missing parts of the shader template
	 *
	 * @return Compiled program sources as (name, source) pairs
	 */
	std::vector<std::pair<std::string, std::string>>
	get_source(GLenum type, const std::vector<std::unique_ptr<basic_part>> &parts) const;

	/**
	 * @brief Get a precompiled shader object for a given type, if available
	 *
	 * @param type Shader type to fetch
	 *
	 * @return Precompiled shader object, or null if no such shader is available
	 */
	std::shared_ptr<backends::gx::shader> get_precompiled(GLenum type) const;

	/**
	 * @brief Compiles a fully specified shader in the cache of this program_template
	 *
	 * @param type Type of the shader to precompile
	 */
	void compile(GLenum type);

	/**
	 * @brief Get the list of supported shader define objects
	 *
	 * @return Reference to the definition objects
	 */
	inline std::map<std::string, std::shared_ptr<preprocessor_defines>> &shader_defines()
	{
		return shader_defines_;
	}

	/**
	 * @brief Get the list of supported shader define objects
	 *
	 * @return Reference to the definition objects
	 */
	inline const std::map<std::string, std::shared_ptr<preprocessor_defines>> &shader_defines() const
	{
		return shader_defines_;
	}
};
} // namespace compiler
} // namespace shadertoy

#endif /* _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_ */

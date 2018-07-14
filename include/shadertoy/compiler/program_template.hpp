#ifndef _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_
#define _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/define_part.hpp"
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

	/**
	 * @brief List of input objects to bind when creating new programs
	 */
	std::map<std::string, basic_shader_inputs *> shader_inputs_;

	/**
	 * @brief List of preprocessor definition objects
	 */
	std::map<std::string, std::shared_ptr<preprocessor_defines>> shader_defines_;

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

	/**
	 * @brief Get the list of supported shader inputs
	 *
	 * The raw pointers in this list should remain valid for as long as this object
	 * is in use to compile programs.
	 *
	 * @return Reference to the shader input list
	 */
	inline std::map<std::string, basic_shader_inputs*> &shader_inputs()
	{ return shader_inputs_; }

	/**
	 * @brief Get the list of supported shader inputs
	 *
	 * The raw pointers in this list should remain valid for as long as this object
	 * is in use to compile programs.
	 *
	 * @return Reference to the shader input list
	 */
	inline const std::map<std::string, basic_shader_inputs *> &shader_inputs() const
	{ return shader_inputs_; }

	/**
	 * @brief Get the list of supported shader define objects
	 *
	 * @return Reference to the definition objects
	 */
	inline std::map<std::string, std::shared_ptr<preprocessor_defines>> &shader_defines()
	{ return shader_defines_; }

	/**
	 * @brief Get the list of supported shader define objects
	 *
	 * @return Reference to the definition objects
	 */
	inline const std::map<std::string, std::shared_ptr<preprocessor_defines>> &shader_defines() const
	{ return shader_defines_; }

	/**
	 * @brief Binds the inputs from this template to a given program
	 *
	 * @param program Compiled program to bind inputs to
	 *
	 * @return List of bound input objects to apply uniform values at runtime
	 */
	std::vector<std::unique_ptr<bound_inputs_base>> bind_inputs(const gl::program &program) const;
};
}
}

#endif /* _SHADERTOY_COMPILER_PROGRAM_TEMPLATE_HPP_ */

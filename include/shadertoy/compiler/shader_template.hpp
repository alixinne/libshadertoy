#ifndef _SHADERTOY_COMPILER_SHADER_TEMPLATE_HPP_
#define _SHADERTOY_COMPILER_SHADER_TEMPLATE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/basic_part.hpp"

#include "shadertoy/polymorphic_value.h"
#include <deque>

namespace shadertoy
{
namespace compiler
{

/**
 * @brief Represents a template to build a shader from source parts
 *
 * The shader_template class is the basis for building fragment shaders from
 * partial sources that should be combined together. For example, toy_buffer
 * instances only specify the `mainImage' entry point, but this should be
 * completed by the definition of the fragmen entry point, as well as uniform
 * variables and samplers. This can be done by adding the corresponding parts
 * to a shader_template.
 *
 * A shader_template is made of parts that can either be specified or not.
 * A specified part is a part which has a name and source code to include in
 * the result. An unspecified part is a part which has only a name, and is used
 * as a placeholder.
 *
 * In order to generate the sources for a shader, all parts of a
 * shader_template instance must be specified. This is done through calling the
 * shader_template#specify method, which returns a copy of the current
 * shader_template by replacing unspecified parts with the provided arguments.
 *
 * Note that part names must be unique within a shader_template.
 */
class shadertoy_EXPORT shader_template
{
	/// List of parts of this template
	std::deque<jbcoe::polymorphic_value<basic_part>> parts_;

	explicit shader_template(std::deque<jbcoe::polymorphic_value<basic_part>> parts);

	void check_unique(jbcoe::polymorphic_value<basic_part> part);

public:
	/**
	 * @brief Initialize a new empty shader_template
	 */
	shader_template();

	/**
	 * @brief Initialize a new shader_template with the given set of parts
	 *
	 * @param parts Initial set of template parts
	 */
	shader_template(std::initializer_list<jbcoe::polymorphic_value<basic_part>> parts);

	/**
	 * @brief Obtain a list of sources from this template
	 *
	 * @return Vector containing the resolved sources of each part in this template
	 *
	 * @throws template_error When some parts are undefined
	 */
	std::vector<std::pair<std::string, std::string>> sources() const;

	/**
	 * @brief Specify some parts in this template
	 *
	 * This methods attempts to replace unspecified parts with parts given as
	 * arguments to this method, in order to make a more specified template.
	 *
	 * @param parts List of parts to define
	 *
	 * @return Copy of this template with unspecified replaced with specified parts from \p parts
	 */
	shader_template specify(std::initializer_list<jbcoe::polymorphic_value<basic_part>> parts) const;

	/**
	 * @brief Add a part to this template
	 *
	 * The part will be added to the end of this template.
	 *
	 * @param part Part to add to this template
	 *
	 * @throws template_error When a part with the same name as \p part already exists
	 */
	void push_back(jbcoe::polymorphic_value<basic_part> part);

	/**
	 * @brief Replace a part from this template
	 *
	 * @param name Name of the part to replace
	 * @param part Part to add to this template
	 *
	 * @throws template_error When a part with the name \p name does not exist
	 * @throws template_error When a part with the same name as \p part already exists
	 */
	void replace(const std::string &name, jbcoe::polymorphic_value<basic_part> part);

	/**
	 * @brief Add a part to this template, before another template part
	 *
	 * The part will be added before the \p target part name, or an error will be thrown.
	 *
	 * @param target Name of the target template part for insertion
	 * @param part   Part to add to this template
	 *
	 * @throws template_error When a part with the same name as \p part already exists
	 * @throws template_error When a part with the \p target name could not be found
	 */
	void insert_before(const std::string &target, jbcoe::polymorphic_value<basic_part> part);

	/**
	 * @brief Add a part to this template, before another template part
	 *
	 * The part will be added after the \p target part name, or an error will be thrown.
	 *
	 * @param target Name of the target template part for insertion
	 * @param part   Part to add to this template
	 *
	 * @throws template_error When a part with the same name as \p part already exists
	 * @throws template_error When a part with the \p target name could not be found
	 */
	void insert_after(const std::string &target, jbcoe::polymorphic_value<basic_part> part);

	/**
	 * @brief Remove a part from this template
	 *
	 * @param name Name of the part to remove
	 *
	 * @return true if the part was removed, false if no such part was found
	 */
	bool erase(const std::string &name);
};
}
}

#endif /* _SHADERTOY_COMPILER_SHADER_TEMPLATE_HPP_ */

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

class shadertoy_EXPORT shader_template
{
	/// List of parts of this template
	std::deque<jbcoe::polymorphic_value<basic_part>> parts_;

	explicit shader_template(std::deque<jbcoe::polymorphic_value<basic_part>> parts);

	void check_unique(jbcoe::polymorphic_value<basic_part> part);

public:
	/**
	 * @brief Initializes a new empty shader_template
	 */
	shader_template();

	/**
	 * @brief Initializes a new shader_template with the given set of parts
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
	 * @brief Specifies some parts in this template
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
	void add_part(jbcoe::polymorphic_value<basic_part> part);

	/**
	 * @brief Add a part to this template, before another template part
	 *
	 * The part will be added before the \p target part name, or an error will be thrown.
	 *
	 * @param part   Part to add to this template
	 * @param target Name of the target template part for insertion
	 *
	 * @throws template_error When a part with the same name as \p part already exists
	 * @throws template_error When a part with the \p target name could not be found
	 */
	void insert_before(jbcoe::polymorphic_value<basic_part> part, const std::string &target);

	/**
	 * @brief Add a part to this template, before another template part
	 *
	 * The part will be added after the \p target part name, or an error will be thrown.
	 *
	 * @param part   Part to add to this template
	 * @param target Name of the target template part for insertion
	 *
	 * @throws template_error When a part with the same name as \p part already exists
	 * @throws template_error When a part with the \p target name could not be found
	 */
	void insert_after(jbcoe::polymorphic_value<basic_part> part, const std::string &target);
};
}
}

#endif /* _SHADERTOY_COMPILER_SHADER_TEMPLATE_HPP_ */

#ifndef _SHADERTOY_COMPILER_SHADER_TEMPLATE_HPP_
#define _SHADERTOY_COMPILER_SHADER_TEMPLATE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/template_part.hpp"

#include <deque>

namespace shadertoy
{
namespace compiler
{

class shadertoy_EXPORT shader_template
{
	/// List of parts of this template
	std::deque<template_part> parts_;

	explicit shader_template(std::deque<template_part> parts);

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
	shader_template(std::initializer_list<template_part> parts);

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
	shader_template specify(std::initializer_list<template_part> parts) const;

	/**
	 * @brief Add a part to this template
	 *
	 * The part will be added to the end of this template.
	 *
	 * @param part Part to add to this template
	 *
	 * @throws template_error When a part with the same name as \p part already exists
	 */
	void add_part(const template_part &part);
};
}
}

#endif /* _SHADERTOY_COMPILER_SHADER_TEMPLATE_HPP_ */

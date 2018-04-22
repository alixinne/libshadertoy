#ifndef _SHADERTOY_COMPILER_INPUT_PART_HPP_
#define _SHADERTOY_COMPILER_INPUT_PART_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/basic_part.hpp"

#include "shadertoy/program_input.hpp"

#include <deque>

namespace shadertoy
{
namespace compiler
{

class shadertoy_EXPORT input_part : public basic_part
{
	/// Referenced input list
	const std::deque<program_input> &inputs_;

public:
	/**
	 * @brief Initializes a new instance of the input_part class
	 *
	 * The given inputs are used to generate the source.
	 *
	 * @param name   Name of this template part
	 * @param inputs List of inputs to generate definitions for
	 */
	input_part(const std::string &name, const std::deque<program_input> &inputs);

	/**
	 * @brief Converts this template_part to a boolean.
	 *
	 * @return true if the template_part is specified, false otherwise
	 */
	operator bool() const override;

	/**
	 * @brief Obtains this template part's sources
	 *
	 * @return Sources for this template part
	 *
	 * @throws template_error If this template's source is not defined
	 */
	std::vector<std::pair<std::string, std::string>> sources() const override;
};
}
}

#endif /* _SHADERTOY_COMPILER_INPUT_PART_HPP_ */

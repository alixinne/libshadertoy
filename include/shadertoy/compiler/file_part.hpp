#ifndef _SHADERTOY_COMPILER_FILE_PART_HPP_
#define _SHADERTOY_COMPILER_FILE_PART_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/basic_part.hpp"

namespace shadertoy
{
namespace compiler
{

/**
 * @brief Represents a part of a shader template which reads its sources from files.
 */
class shadertoy_EXPORT file_part : public cloneable_part<file_part>
{
	/// Source file for this template
	std::string source_file_;

public:
	/**
	 * @brief Initialize a new unspecified file_part
	 *
	 * @param name Name of this template part
	 */
	file_part(const std::string &name);

	/**
	 * @brief Initialize a new specified file_part
	 *
	 * @param name        Name of this template part
	 * @param source_file Source file for this template part
	 */
	file_part(const std::string &name, const std::string &source_file);

	/**
	 * @brief Convert this file_part to a boolean.
	 *
	 * @return true if the file_part is specified, false otherwise
	 */
	operator bool() const override;

	/**
	 * @brief Obtain this template part's sources
	 *
	 * @return Sources for this template part
	 *
	 * @throws template_error If this template's source is not defined
	 */
	std::vector<std::pair<std::string, std::string>> sources() const override;
};
}
}

#endif /* _SHADERTOY_COMPILER_FILE_PART_HPP_ */

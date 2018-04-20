#ifndef _SHADERTOY_COMPILER_TEMPLATE_PART_HPP_
#define _SHADERTOY_COMPILER_TEMPLATE_PART_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/basic_part.hpp"

namespace shadertoy
{
namespace compiler
{

/**
 * @brief Represents a part of a shader template.
 */
class shadertoy_EXPORT template_part : public basic_part
{
	/// Sources for this template part, with their names
	std::vector<std::pair<std::string, std::string>> sources_;

	/// true if this template part is specified
	bool has_sources_;

public:
	/**
	 * @brief Initializes a new unspecified template_part
	 *
	 * @param name Name of this template part
	 */
	template_part(const std::string &name);

	/**
	 * @brief Initializes a new specified template_part
	 *
	 * @param name   Name of this template part
	 * @param source Source for this template part
	 */
	template_part(const std::string &name, const std::string &source);

	/**
	 * @brief Initializes a new specified template_part
	 *
	 * @param name    Name of this template part
	 * @param sources Sources for this template part
	 */
	template_part(const std::string &name, const std::vector<std::pair<std::string, std::string>> &sources);

	/**
	 * @brief Initializes a new specified template_part
	 *
	 * @param name     Name of this template part
	 * @param filename Filename of the file to load the source from
	 *
	 * @return New template_part object
	 *
	 * @throws template_error When reading the template file fails
	 */
	static template_part from_file(const std::string &name, const std::string &filename);

	/**
	 * @brief Initializes a new specified template_part
	 *
	 * @param name      Name of this template part
	 * @param filenames List of filenames to load sources from
	 *
	 * @return New template_part object
	 *
	 * @throws template_error When reading a template file fails
	 */
	static template_part from_files(const std::string &name, const std::vector<std::string> &filenames);

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

#endif /* _SHADERTOY_COMPILER_TEMPLATE_PART_HPP_ */

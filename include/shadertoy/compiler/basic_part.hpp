#ifndef _SHADERTOY_COMPILER_BASIC_PART_HPP_
#define _SHADERTOY_COMPILER_BASIC_PART_HPP_

#include "shadertoy/pre.hpp"

#include <string>
#include <utility>
#include <vector>

namespace shadertoy
{
namespace compiler
{

class shadertoy_EXPORT basic_part
{
	/// Name of this template part
	std::string name_;

protected:
	/**
	 * @brief Initializes a new instance of basic_part
	 *
	 * @param name Name of this part
	 */
	basic_part(const std::string &name);

public:
	/**
	 * @brief Obtains this template part's name
	 *
	 * @return Name of this template part
	 */
	inline const std::string &name() const
	{ return name_; }

	/**
	 * @brief See operator bool()
	 */
	inline bool specified() const
	{ return static_cast<bool>(*this); }

	/**
	 * @brief Converts this template_part to a boolean.
	 *
	 * @return true if the template_part is specified, false otherwise
	 */
	virtual operator bool() const = 0;

	/**
	 * @brief Obtains this template part's sources
	 *
	 * @return Sources for this template part
	 *
	 * @throws template_error If this template's source is not defined
	 */
	virtual std::vector<std::pair<std::string, std::string>> sources() const = 0;
};
}
}

#endif /* _SHADERTOY_COMPILER_BASIC_PART_HPP_ */

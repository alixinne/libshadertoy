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

/**
 * @brief Base class representing a part of a shader_template
 */
class shadertoy_EXPORT basic_part
{
	/// Name of this template part
	std::string name_;

protected:
	/**
	 * @brief Initialize a new instance of basic_part
	 *
	 * @param name Name of this part
	 */
	basic_part(const std::string &name);

public:
	virtual ~basic_part();

	/**
	 * @brief Obtain this template part's name
	 *
	 * @return Name of this template part
	 */
	inline const std::string &name() const
	{ return name_; }

	/**
	 * @brief Get a value indicating if this template_part is specified
	 *
	 * @return true if the template_part is specified, false otherwise
	 */
	inline bool specified() const
	{ return static_cast<bool>(*this); }

	/**
	 * @brief Convert this template_part to a boolean.
	 *
	 * @return true if the template_part is specified, false otherwise
	 */
	virtual operator bool() const = 0;

	/**
	 * @brief Obtain this template part's sources
	 *
	 * @return Sources for this template part. The first element of each pair is the name of the source part,
	 * and the second element is the GLSL source contents. The first element
	 * may be the name of the basic_part instance.
	 *
	 * @throws template_error If this template's source is not defined
	 */
	virtual std::vector<std::pair<std::string, std::string>> sources() const = 0;

	/**
	 * @brief Clones this part for use in a new shader template
	 *
	 * @return Clone of the current instance
	 */
	virtual basic_part *clone() const = 0;
};

/**
 * @brief Implements clone functionality for template parts
 */
template<typename defined_part>
class cloneable_part : public basic_part
{
protected:
	cloneable_part(const std::string &name)
		: basic_part(name)
	{
	}

public:
	virtual basic_part *clone() const
	{
		return new defined_part(static_cast<const defined_part &>(*this));
	}

	/**
	 * @brief Clones the current part into a unique_ptr
	 *
	 * @return unique_ptr pointing to the cloned instance
	 */
	operator std::unique_ptr<basic_part>() const
	{
		return std::unique_ptr<basic_part>(clone());
	}
};
}
}

#endif /* _SHADERTOY_COMPILER_BASIC_PART_HPP_ */

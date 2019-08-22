#ifndef _SHADERTOY_COMPILER_DEFINE_PART_HPP_
#define _SHADERTOY_COMPILER_DEFINE_PART_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/compiler/basic_part.hpp"

#include <map>
#include <memory>
#include <string>

namespace shadertoy
{
namespace compiler
{

/**
 * @brief Represents a set of GLSL preprocessor definitions
 */
class shadertoy_EXPORT preprocessor_defines
{
	/// Generated source
	std::string generated_source_;

	/// true if the source has been generated
	bool source_ready_;

	/// List of preprocessor definitions
	std::map<std::string, std::string> definitions_;

	public:
	/**
	 * @brief Initialize a new instance of the preprocessor_defines class
	 */
	preprocessor_defines();

	/**
	 * @brief Obtain a non-const reference to the definitions map
	 *
	 * @return Non-const reference to the definitions map
	 */
	inline std::map<std::string, std::string> &definitions()
	{
		source_ready_ = false;
		return definitions_;
	}

	/**
	 * @brief Obtain a const reference to the definitions map
	 *
	 * @return Const reference to the definitions map
	 */
	inline const std::map<std::string, std::string> &definitions() const { return definitions_; }

	/**
	 * @brief Obtain the source corresponding to the definitions in this object
	 *
	 * @return GLSL Source including the definitions as a string
	 */
	const std::string &source();
};

/**
 * @brief Represents a shader template part that defines preprocessor variables
 */
class shadertoy_EXPORT define_part : public cloneable_part<define_part>
{
	/// Preprocessor defines block
	std::shared_ptr<preprocessor_defines> definitions_;

	public:
	/**
	 * @brief Initialize a new instance of the define_part class
	 *
	 * A new empty preprocessor define block is created.
	 *
	 * @param name Name of this template part
	 */
	define_part(const std::string &name);

	/**
	 * @brief Initialize a new instance of the define_part class
	 *
	 * The given preprocessor define block is used. It must be non-null.
	 *
	 * @param name    Name of this template part
	 * @param defines Preprocessor definition block
	 */
	define_part(const std::string &name, const std::shared_ptr<preprocessor_defines> &defines);

	/**
	 * @brief Obtain the preprocessor definition block
	 *
	 * @return Reference to the preprocessor define block
	 */
	inline const std::shared_ptr<preprocessor_defines> &definitions() const { return definitions_; }

	/**
	 * @brief Convert this template_part to a boolean.
	 *
	 * @return true if the template_part is specified, false otherwise
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
} // namespace compiler
} // namespace shadertoy

#endif /* _SHADERTOY_COMPILER_DEFINE_PART_HPP_ */

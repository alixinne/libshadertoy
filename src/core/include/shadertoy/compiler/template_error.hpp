#ifndef _SHADERTOY_COMPILER_TEMPLATE_ERROR_HPP_
#define _SHADERTOY_COMPILER_TEMPLATE_ERROR_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/shadertoy_error.hpp"

namespace shadertoy
{
namespace compiler
{

/**
 * @brief Error thrown on invalid use of a shader_template
 */
class shadertoy_EXPORT template_error : public shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the template_error class
	 *
	 * @param what Message describing the template exception
	 */
	explicit template_error(const std::string &what);
};
} // namespace compiler
} // namespace shadertoy

#endif

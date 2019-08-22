#ifndef _SHADERTOY_SHADERTOY_ERROR_HPP_
#define _SHADERTOY_SHADERTOY_ERROR_HPP_

#include <stdexcept>
#include <string>

namespace shadertoy
{
/**
 * @brief Base error class for libshadertoy errors.
 */
class shadertoy_error : public std::runtime_error
{
	public:
	/**
	 * @brief Initialize a new instance of the ShadertoyError class.
	 *
	 * @param  what_arg Error message
	 */
	explicit shadertoy_error(const std::string &what_arg);
	/**
	 * @brief Initialize a new instance of the ShadertoyError class.
	 *
	 * @param  what_arg Error message
	 */
	explicit shadertoy_error(const char *what_arg);
};
} // namespace shadertoy

#endif /* _SHADERTOY_SHADERTOY_ERROR_HPP_ */

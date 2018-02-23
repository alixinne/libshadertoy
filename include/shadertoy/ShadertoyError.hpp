#ifndef _SHADERTOY_SHADERTOY_ERROR_HPP_
#define _SHADERTOY_SHADERTOY_ERROR_HPP_

#include "shadertoy/pre.hpp"

#include <stdexcept>

namespace shadertoy
{
/**
 * @brief Base error class for libshadertoy errors.
 */
class ShadertoyError : public std::runtime_error
{
public:
	/**
	 * @brief Initializes a new instance of the ShadertoyError class.
	 *
	 * @param  what_arg Error message
	 */
	explicit ShadertoyError(const std::string &what_arg);
	/**
	 * @brief Initializes a new instance of the ShadertoyError class.
	 *
	 * @param  what_arg Error message
	 */
	explicit ShadertoyError(const char *what_arg);
};
}

#endif /* _SHADERTOY_SHADERTOY_ERROR_HPP_ */

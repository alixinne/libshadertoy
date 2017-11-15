#ifndef _SHADERTOY_SHADERTOY_ERROR_HPP_
#define _SHADERTOY_SHADERTOY_ERROR_HPP_

#include "shadertoy/pre.hpp"

#include <stdexcept>

namespace shadertoy
{
class ShadertoyError : public std::runtime_error
{
public:
	explicit ShadertoyError(const std::string &what_arg);
	explicit ShadertoyError(const char *what_arg);
};
}

#endif /* _SHADERTOY_SHADERTOY_ERROR_HPP_ */

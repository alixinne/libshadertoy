#include "shadertoy/ShadertoyError.hpp"

using namespace shadertoy;

ShadertoyError::ShadertoyError(const std::string &what_arg)
	: std::runtime_error(what_arg)
{
}

ShadertoyError::ShadertoyError(const char *what_arg)
	: std::runtime_error(what_arg)
{
}

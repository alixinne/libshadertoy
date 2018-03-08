#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy;

shadertoy_error::shadertoy_error(const std::string &what_arg)
	: std::runtime_error(what_arg)
{
}

shadertoy_error::shadertoy_error(const char *what_arg)
	: std::runtime_error(what_arg)
{
}

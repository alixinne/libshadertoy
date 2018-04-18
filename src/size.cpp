#include "shadertoy/pre.hpp"

#include "shadertoy/size.hpp"

using namespace shadertoy;

size_error::size_error(const std::string &what) : shadertoy_error(what) {}

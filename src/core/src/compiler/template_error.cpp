#include "shadertoy/compiler/template_error.hpp"

using namespace shadertoy;
using namespace shadertoy::compiler;

template_error::template_error(const std::string &what) : shadertoy_error(what) {}

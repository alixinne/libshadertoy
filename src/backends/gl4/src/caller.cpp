#include <epoxy/gl.h>

#include "shadertoy/backends/gl4/caller.hpp"
#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/backends/gl4/backend.hpp"

using namespace shadertoy::backends::gl4;

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
void check_errors() { backend::current().check_errors(); }
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

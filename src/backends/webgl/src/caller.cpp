#include <GLES3/gl3.h>

#include "shadertoy/backends/webgl/caller.hpp"
#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/backends/webgl/backend.hpp"

using namespace shadertoy::backends::webgl;

SHADERTOY_BACKENDS_WEBGL_NAMESPACE_BEGIN
void check_errors() { backend::current().check_errors(); }
SHADERTOY_BACKENDS_WEBGL_NAMESPACE_END

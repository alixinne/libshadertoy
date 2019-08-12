#include <epoxy/gl.h>

#include "shadertoy/gl/caller.hpp"
#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/gl/wrapper_context.hpp"

using namespace shadertoy::gl;

namespace shadertoy
{
namespace gl
{
	thread_local wrapper_context current_context;

	void check_errors()
	{
		current_context.check_errors();
	}
}  // namespace gl
}  // namespace shadertoy

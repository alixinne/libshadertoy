#include <epoxy/gl.h>

#include "shadertoy/gl/vertex_array.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::gl;

null_vertex_array_error::null_vertex_array_error()
	: shadertoy_error("An attempt was made to dereference a null vertex_array")
{
}

void vertex_array::bind() const
{
	gl_call(glBindVertexArray, GLuint(*this));
}

void vertex_array::unbind() const
{
	gl_call(glBindVertexArray, 0);
}

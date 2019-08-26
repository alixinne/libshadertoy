#include <GLES3/gl3.h>

#include "shadertoy/backends/webgl/vertex_array.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::backends::webgl;

null_vertex_array_error::null_vertex_array_error()
: shadertoy_error("An attempt was made to dereference a null vertex_array")
{
}

void vertex_array::bind() const { gl_call(glBindVertexArray, GLuint(*this)); }

void vertex_array::unbind() const { gl_call(glBindVertexArray, 0); }

void vertex_array::draw_elements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) const
{
	bind();
	gl_call(glDrawElements, mode, count, type, indices);
}

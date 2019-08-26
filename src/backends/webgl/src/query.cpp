#include <GLES3/gl3.h>

#include "shadertoy/backends/webgl/query.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::backends::webgl;

null_query_error::null_query_error()
: shadertoy_error("An attempt was made to dereference a null query")
{
}

void query::begin(GLenum target) const { gl_call(glBeginQuery, target, GLuint(*this)); }

void query::end(GLenum target) const { gl_call(glEndQuery, target); }

void query::get_object_iv(GLenum pname, GLint *params) const
{
	//TODO: fix this function
	gl_call(glGetQueryObjectuiv, GLuint(*this), pname, reinterpret_cast<GLuint*>(params));
}

void query::get_object_ui64v(GLenum pname, GLuint64 *params) const
{
	//TODO: disable this function
	gl_call(glGetQueryObjectuiv, GLuint(*this), pname, reinterpret_cast<GLuint*>(params));
}

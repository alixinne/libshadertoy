#include <epoxy/gl.h>

#include "shadertoy/backends/gl4/query.hpp"
#include "shadertoy/shadertoy_error.hpp"

using namespace shadertoy::backends::gl4;

null_query_error::null_query_error()
: shadertoy_error("An attempt was made to dereference a null query")
{
}

GLuint query_allocator::create(GLenum target)
{
	GLuint id;
	gl_call(glCreateQueries, target, 1, &id);
	return id;
}

void query_allocator::destroy(GLuint resource) { gl_call(glDeleteQueries, 1, &resource); }

query::query(GLenum target) : resource(allocator_type().create(target)) {}

void query::begin(GLenum target) const { gl_call(glBeginQuery, target, GLuint(*this)); }

void query::end(GLenum target) const { gl_call(glEndQuery, target); }

void query::query_counter(GLenum target) const { gl_call(glQueryCounter, GLuint(*this), target); }

void query::get_object_iv(GLenum pname, GLint *params) const
{
	gl_call(glGetQueryObjectiv, GLuint(*this), pname, params);
}

void query::get_object_ui64v(GLenum pname, GLuint64 *params) const
{
	gl_call(glGetQueryObjectui64v, GLuint(*this), pname, params);
}

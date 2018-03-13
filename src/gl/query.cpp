#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/query.hpp"

using namespace shadertoy::gl;

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

void query_allocator::destroy(GLuint resource)
{
	gl_call(glDeleteQueries, 1, &resource);
}

query::query(GLenum target)
	: resource(allocator_type().create(target))
{
}

void query::begin(GLenum target)
{
	gl_call(glBeginQuery, target, GLuint(*this));
}

void query::end(GLenum target)
{
	gl_call(glEndQuery, target);
}

void query::query_counter(GLenum target)
{
	gl_call(glQueryCounter, GLuint(*this), target);
}

void query::get_object_iv(GLenum pname, GLint *params)
{
	gl_call(glGetQueryObjectiv, GLuint(*this), pname, params);
}

void query::get_object_ui64v(GLenum pname, GLuint64 *params)
{
	gl_call(glGetQueryObjectui64v, GLuint(*this), pname, params);
}

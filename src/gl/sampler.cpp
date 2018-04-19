#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/sampler.hpp"

using namespace shadertoy::gl;

null_sampler_error::null_sampler_error()
	: shadertoy_error("An attempt was made to dereference a null sampler")
{
}

void sampler::bind(GLuint unit)
{
	gl_call(glBindSampler, unit, GLuint(*this));
}

void sampler::parameter(GLenum pname, GLint param)
{
	gl_call(glSamplerParameteri, GLuint(*this), pname, param);
}

void sampler::parameter(GLenum pname, GLfloat param)
{
	gl_call(glSamplerParameterf, GLuint(*this), pname, param);
}

void sampler::get_parameter(GLenum pname, GLint *param)
{
	gl_call(glGetSamplerParameteriv, GLuint(*this), pname, param);
}

void sampler::get_parameter(GLenum pname, GLfloat *param)
{
	gl_call(glGetSamplerParameterfv, GLuint(*this), pname, param);
}

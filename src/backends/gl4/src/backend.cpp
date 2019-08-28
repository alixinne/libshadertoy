#include <epoxy/gl.h>

#include "shadertoy/backends/gl4/backend.hpp"

#include "shadertoy/backends/gl4/buffer.hpp"
#include "shadertoy/backends/gl4/draw_state.hpp"
#include "shadertoy/backends/gl4/framebuffer.hpp"
#include "shadertoy/backends/gl4/program.hpp"
#include "shadertoy/backends/gl4/query.hpp"
#include "shadertoy/backends/gl4/renderbuffer.hpp"
#include "shadertoy/backends/gl4/sampler.hpp"
#include "shadertoy/backends/gl4/shader.hpp"
#include "shadertoy/backends/gl4/texture.hpp"
#include "shadertoy/backends/gl4/vertex_array.hpp"

using namespace shadertoy::backends;
using namespace shadertoy::backends::gl4;

void backend_ops::GetIntegerv(GLenum pname, GLint *params) const
{
	return glGetIntegerv(pname, params);
}

GLenum backend_ops::GetError() const { return glGetError(); }

void backend_ops::UseProgram(GLuint program) const
{
	gl_call(glUseProgram, program);
}

void backend_ops::bind_texture_unit(GLuint unit, std::optional<std::reference_wrapper<const gx::texture>> texture) const
{
	GLuint texture_id = texture == std::nullopt ? 0 : GLuint(texture->get());
	gl_call(glBindTextureUnit, unit, texture_id);
}

void backend_ops::bind_sampler_unit(GLuint unit, std::optional<std::reference_wrapper<const gx::sampler>> sampler) const
{
	GLuint sampler_id = sampler == std::nullopt ? 0 : GLuint(sampler->get());
	gl_call(glBindSampler, unit, sampler_id);
}

void backend_ops::bind_texture(GLenum target, std::optional<std::reference_wrapper<const gx::texture>> texture) const
{
	GLuint texture_id = texture == std::nullopt ? 0 : GLuint(texture->get());
	gl_call(glBindTexture, target, texture_id);
}

void backend_ops::active_texture(GLuint unit) const { gl_call(glActiveTexture, unit); }

backend::backend(bool state_tracking, bool checks_enabled)
: gx::state_tracker<backend_ops, gx::backend>(backend_ops(), state_tracking, checks_enabled)
{
}

std::unique_ptr<gx::buffer> backend::make_buffer(GLenum _target)
{
	return std::make_unique<buffer>();
}

std::unique_ptr<gx::framebuffer> backend::make_framebuffer()
{
	return std::make_unique<framebuffer>();
}

std::unique_ptr<gx::program> backend::make_program() { return std::make_unique<program>(); }

std::unique_ptr<gx::attrib_location> backend::make_attrib_location(GLint location)
{
	return std::make_unique<attrib_location>(location);
}

std::unique_ptr<gx::uniform_location> backend::make_uniform_location(const gx::program &program, GLint location)
{
	return std::make_unique<uniform_location>(program, location);
}

std::unique_ptr<gx::query> backend::make_query(GLenum target)
{
	return std::make_unique<query>(target);
}

std::unique_ptr<gx::renderbuffer> backend::make_renderbuffer()
{
	return std::make_unique<renderbuffer>();
}

std::unique_ptr<gx::sampler> backend::make_sampler() { return std::make_unique<sampler>(); }

std::unique_ptr<gx::shader> backend::make_shader(GLenum shaderType)
{
	return std::make_unique<shader>(shaderType);
}

std::unique_ptr<gx::texture> backend::make_texture(GLenum target)
{
	return std::make_unique<texture>(target);
}

std::unique_ptr<gx::vertex_array> backend::make_vertex_array()
{
	return std::make_unique<vertex_array>();
}

std::unique_ptr<gx::draw_state> backend::make_draw_state()
{
	return std::make_unique<draw_state>();
}

void backend::bind_default_framebuffer(GLenum target) { gl_call(glBindFramebuffer, target, 0); }

void backend::get_viewport(GLint viewport[4]) const
{
	glGetIntegerv(GL_VIEWPORT, std::addressof(viewport[0]));
}

void backend::set_viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	gl_call(glViewport, x, y, width, height);
}

backend &backend::current()
{
	// Unsafe, but avoids dynamic_cast.
	return *reinterpret_cast<backend *>(backends::current().get());
}

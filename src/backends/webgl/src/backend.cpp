#include <GLES3/gl3.h>

#include "shadertoy/backends/webgl/backend.hpp"

#include "shadertoy/backends/webgl/buffer.hpp"
#include "shadertoy/backends/webgl/draw_state.hpp"
#include "shadertoy/backends/webgl/framebuffer.hpp"
#include "shadertoy/backends/webgl/program.hpp"
#include "shadertoy/backends/webgl/query.hpp"
#include "shadertoy/backends/webgl/renderbuffer.hpp"
#include "shadertoy/backends/webgl/sampler.hpp"
#include "shadertoy/backends/webgl/shader.hpp"
#include "shadertoy/backends/webgl/texture.hpp"
#include "shadertoy/backends/webgl/vertex_array.hpp"

using namespace shadertoy::backends;
using namespace shadertoy::backends::webgl;

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
	GLuint texture_target =
	texture == std::nullopt ? 0 : reinterpret_cast<const webgl::texture *>(&texture->get())->target();

	backend_active_texture(unit);
	backend_bind_texture(texture_target, texture);
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

backend_ops::backend_ops(std::function<void(GLuint)> backend_active_texture,
						 std::function<void(GLenum, std::optional<std::reference_wrapper<const gx::texture>>)> backend_bind_texture)
: backend_active_texture(backend_active_texture), backend_bind_texture(backend_bind_texture)
{
}

backend::backend(bool state_tracking, bool checks_enabled)
: gx::state_tracker<backend_ops, gx::backend>(
  backend_ops([this](GLuint unit) { this->active_texture(unit); },
			  [this](GLenum target, std::optional<std::reference_wrapper<const gx::texture>> texture) {
				  this->bind_texture(target, texture);
			  }),
  state_tracking, state_tracking)
{
}

std::unique_ptr<gx::buffer> backend::make_buffer(GLenum target)
{
	return std::make_unique<buffer>(target);
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

std::unique_ptr<gx::query> backend::make_query(GLenum target) { return std::make_unique<query>(); }

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

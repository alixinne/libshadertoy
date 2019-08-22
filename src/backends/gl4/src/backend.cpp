#include <epoxy/gl.h>

#include "shadertoy/backends/gl4/backend.hpp"
#include "shadertoy/backends/gl4/opengl_error.hpp"

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

void backend::init_texture_unit_mappings()
{
	if (texture_unit_mappings_.empty())
	{
		GLint max_combined_texture_image_units;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);

		texture_unit_mappings_.resize(max_combined_texture_image_units, 0);
	}
}

void backend::check_texture_unit(GLuint unit) const
{
	if (unit >= texture_unit_mappings_.size())
	{
		throw opengl_error(GL_INVALID_VALUE, "Texture unit ID is out of range");
	}
}

void backend::check_errors() const
{
	if (!checks_enabled_)
		return;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		throw opengl_error(error, std::string());
	}
}

void backend::bind_texture_unit(GLuint unit, std::optional<std::reference_wrapper<const gl4::texture>> texture)
{
	GLuint texture_id = texture == std::nullopt ? 0 : GLuint(texture->get());

	if (state_tracking_)
	{
		// Initialize texture mapping table
		init_texture_unit_mappings();

		// Check unit number
		check_texture_unit(unit);

		// Bind texture if necessary
		if (texture_unit_mappings_[unit] != texture_id)
		{
			gl_call(glBindTextureUnit, unit, texture_id);
			texture_unit_mappings_[unit] = texture_id;
		}
	}
	else
	{
		// Not tracking, just forward the call
		gl_call(glBindTextureUnit, unit, texture_id);
	}
}

void backend::unbind_texture_units(GLuint start, int count)
{
	if (!state_tracking_)
		throw opengl_error(GL_INVALID_OPERATION, "This context is not tracking state.");

	for (size_t i = start; i < texture_unit_mappings_.size() && (count < 0 || i < (start + count)); ++i)
	{
		if (texture_unit_mappings_[i] != 0)
		{
			gl_call(glBindTextureUnit, i, 0);
			texture_unit_mappings_[i] = 0;
		}
	}
}

void backend::bind_texture(GLenum target, std::optional<std::reference_wrapper<const gl4::texture>> texture)
{
	GLuint texture_id = texture == std::nullopt ? 0 : GLuint(texture->get());

	if (state_tracking_)
	{
		// Initialize texture mapping table
		init_texture_unit_mappings();

		// Find texture target in mapping table
		auto it = texture_target_mappings_.find(target);

		// Bind texture if necessary
		if (texture_unit_mappings_[active_texture_unit_] != texture_id ||
			it == texture_target_mappings_.end() || it->second != texture_id)
		{
			gl_call(glBindTexture, target, texture_id);
			texture_unit_mappings_[active_texture_unit_] = texture_id;
			texture_unit_mappings_[target] = texture_id;
		}
	}
	else
	{
		// Not tracking, just forward the call
		gl_call(glBindTexture, target, texture_id);
	}
}

void backend::active_texture(GLuint unit)
{
	if (state_tracking_)
	{
		// Initialize texture mapping table
		init_texture_unit_mappings();

		// Check unit number
		check_texture_unit(unit);

		if (unit != active_texture_unit_)
		{
			gl_call(glActiveTexture, GL_TEXTURE0 + unit);
			active_texture_unit_ = unit;
		}
	}
	else
	{
		gl_call(glActiveTexture, GL_TEXTURE0 + unit);
	}
}

backend::backend(bool state_tracking, bool checks_enabled)
: checks_enabled_(checks_enabled), state_tracking_(state_tracking), active_texture_unit_(0)
{
}

std::unique_ptr<gx::buffer> backend::make_buffer()
{
	return std::make_unique<buffer>();
}

std::unique_ptr<gx::framebuffer> backend::make_framebuffer()
{
	return std::make_unique<framebuffer>();
}

std::unique_ptr<gx::program> backend::make_program()
{
	return std::make_unique<program>();
}

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

std::unique_ptr<gx::sampler> backend::make_sampler()
{
	return std::make_unique<sampler>();
}

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

void backend::bind_default_framebuffer(GLenum target)
{
	gl_call(glBindFramebuffer, target, 0);
}

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
	return *reinterpret_cast<backend *>(backends::current.get());
}

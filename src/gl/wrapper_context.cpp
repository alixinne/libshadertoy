#include <epoxy/gl.h>

#include "shadertoy/gl/opengl_error.hpp"
#include "shadertoy/gl/wrapper_context.hpp"

using namespace shadertoy;
using namespace shadertoy::gl;

void wrapper_context::init_texture_unit_mappings()
{
	if (texture_unit_mappings_.empty())
	{
		GLint max_combined_texture_image_units;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);

		texture_unit_mappings_.resize(max_combined_texture_image_units, 0);
	}
}

void wrapper_context::check_texture_unit(GLuint unit) const
{
	if (unit >= texture_unit_mappings_.size())
	{
		throw opengl_error(GL_INVALID_VALUE, "Texture unit ID is out of range");
	}
}

void wrapper_context::check_errors() const
{
	if (!checks_enabled_)
		return;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		throw opengl_error(error, std::string());
	}
}

void wrapper_context::bind_texture_unit(GLuint unit, std::optional<std::reference_wrapper<const gl::texture>> texture)
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

void wrapper_context::bind_texture(GLenum target, std::optional<std::reference_wrapper<const gl::texture>> texture)
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

void wrapper_context::active_texture(GLuint unit)
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

wrapper_context::wrapper_context(bool state_tracking, bool checks_enabled)
: checks_enabled_(checks_enabled), state_tracking_(state_tracking), active_texture_unit_(0)
{
}

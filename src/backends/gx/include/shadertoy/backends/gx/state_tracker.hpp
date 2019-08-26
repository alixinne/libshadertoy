#ifndef _SHADERTOY_BACKENDS_GX_STATE_TRACKER_HPP_
#define _SHADERTOY_BACKENDS_GX_STATE_TRACKER_HPP_

#include "shadertoy/backends/gx/pre.hpp"

#include "shadertoy/backends/gx/opengl_error.hpp"
#include "shadertoy/backends/gx/sampler.hpp"
#include "shadertoy/backends/gx/texture.hpp"

#include <functional>
#include <map>
#include <optional>
#include <vector>

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
template <typename Ops, typename Base> class state_tracker : public Base
{
	/// true if glGetError() is checked
	bool checks_enabled_;

	/// true if this wrapper tracks state
	bool state_tracking_;

	/// Texture unit mappings
	std::vector<GLenum> texture_unit_mappings_;

	/// Texture unit sampler mappings
	std::vector<GLenum> sampler_unit_mappings_;

	/// Active texture unit
	GLenum active_texture_unit_;

	/// Texture target mappings
	std::map<GLenum, GLuint> texture_target_mappings_;

	/// Current program in use
	GLuint current_program_;

	/// OpenGL operations
	Ops ops_;

	void init_texture_unit_mappings()
	{
		GLint max_combined_texture_image_units = 0;

		if (texture_unit_mappings_.empty())
		{
			ops_.GetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);
			texture_unit_mappings_.resize(max_combined_texture_image_units, 0);
		}

		if (sampler_unit_mappings_.empty())
		{
			if (max_combined_texture_image_units == 0)
				ops_.GetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);
			sampler_unit_mappings_.resize(max_combined_texture_image_units, 0);
		}
	}

	void check_texture_unit(GLuint unit) const
	{
		if (unit >= texture_unit_mappings_.size())
		{
			throw opengl_error(GL_INVALID_VALUE, "Texture unit ID is out of range");
		}
	}

	public:
	/**
	 * @brief Throw an opengl_error if glGetError returns non-zero
	 *
	 * @throws opengl_error
	 */
	void check_errors() const
	{
		if (!checks_enabled_)
			return;

		GLenum error = ops_.GetError();
		if (error != GL_NO_ERROR)
		{
			throw opengl_error(error, std::string());
		}
	}

	/**
	 * @brief Sets the flag for enabling or disabling calls to glGetError()
	 *
	 * @param enabled true if glGetError() should be called, false otherwise
	 */
	inline void checks_enabled(bool enabled) { checks_enabled_ = enabled; }

	/**
	 * @brief Bind a texture to a specific unit
	 *
	 * @param unit    Texture unit index (0-based)
	 * @param texture Texture to bind to the unit
	 */
	void bind_texture_unit(GLuint unit, std::optional<std::reference_wrapper<const gx::texture>> texture)
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
				ops_.bind_texture_unit(unit, texture);
				texture_unit_mappings_[unit] = texture_id;
			}
		}
		else
		{
			// Not tracking, just forward the call
			ops_.bind_texture_unit(unit, texture);
		}
	}

	/**
	 * @brief Bind a sampler to a specific texture unit
	 *
	 * @param unit    Texture unit index (0-based)
	 * @param texture Sampler to bind to the unit
	 */
	void bind_sampler_unit(GLuint unit,
						   std::optional<std::reference_wrapper<const gx::sampler>> sampler = std::nullopt)

	{
		GLuint sampler_id = sampler == std::nullopt ? 0 : GLuint(sampler->get());

		if (state_tracking_)
		{
			// Initialize sampler mapping table
			init_texture_unit_mappings();

			// Check unit number
			check_texture_unit(unit);

			// Bind sampler if necessary
			if (sampler_unit_mappings_[unit] != sampler_id)
			{
				ops_.bind_sampler_unit(unit, sampler);
				sampler_unit_mappings_[unit] = sampler_id;
			}
		}
		else
		{
			// Not tracking, just forward the call
			ops_.bind_sampler_unit(unit, sampler);
		}
	}

	/**
	 * @brief Unbind a range of texture units
	 *
	 * Requires state tracking to be enabled.
	 *
	 * @param start First texture unit to unbind
	 * @param count Number of texture units to unbind. -1 for all remaining
	 */
	void unbind_texture_units(GLuint start, int count = -1)
	{
		if (!state_tracking_)
			// throw opengl_error(GL_INVALID_OPERATION, "This context is not tracking state.");
			// TODO: issue warning
			return;

		for (size_t i = start; i < texture_unit_mappings_.size() && (count < 0 || i < (start + count)); ++i)
		{
			if (texture_unit_mappings_[i] != 0)
			{
				ops_.bind_texture_unit(i, std::nullopt);
				texture_unit_mappings_[i] = 0;
			}

			if (sampler_unit_mappings_[i] != 0)
			{
				ops_.bind_sampler_unit(i, std::nullopt);
				sampler_unit_mappings_[i] = 0;
			}
		}
	}

	/**
	 * @brief Bind a texture to a target
	 *
	 * @param target  Texture target
	 * @param texture Texture to bind to the target
	 */
	void bind_texture(GLenum target, std::optional<std::reference_wrapper<const gx::texture>> texture = std::nullopt)

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
				ops_.bind_texture(target, texture);
				texture_unit_mappings_[active_texture_unit_] = texture_id;
				texture_unit_mappings_[target] = texture_id;
			}
		}
		else
		{
			// Not tracking, just forward the call
			ops_.bind_texture(target, texture);
		}
	}

	/**
	 * @brief Activate a texture unit
	 *
	 * @param unit    Texture unit index (0-based)
	 */
	void active_texture(GLuint unit)
	{
		if (state_tracking_)
		{
			// Initialize texture mapping table
			init_texture_unit_mappings();

			// Check unit number
			check_texture_unit(unit);

			if (unit != active_texture_unit_)
			{
				ops_.active_texture(GL_TEXTURE0 + unit);
				active_texture_unit_ = unit;
			}
		}
		else
		{
			ops_.active_texture(GL_TEXTURE0 + unit);
		}
	}

	/**
	 * @brief Sets the current program
	 *
	 * @param program Program name to use
	 */
	void use_program(GLuint program)
	{
		if (state_tracking_)
		{
			if (current_program_ != program)
			{
				ops_.UseProgram(program);
				current_program_ = program;
			}
		}
		else
		{
			ops_.UseProgram(program);
		}
	}

	state_tracker(Ops ops, bool state_tracking = true, bool checks_enabled = true)
	: checks_enabled_(checks_enabled), state_tracking_(state_tracking), active_texture_unit_(0),
	  current_program_(0), ops_(ops)
	{
	}
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_STATE_TRACKER_HPP_ */

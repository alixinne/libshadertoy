#ifndef _SHADERTOY_GL_WRAPPER_CONTEXT_HPP_
#define _SHADERTOY_GL_WRAPPER_CONTEXT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/gl/texture.hpp"

#include <functional>
#include <map>
#include <optional>
#include <vector>

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief OpenGL state tracker
	 */
	class shadertoy_EXPORT wrapper_context
	{
		/// true if glGetError() is checked
		bool checks_enabled_;

		/// true if this wrapper tracks state
		bool state_tracking_;

		/// Texture unit mappings
		std::vector<GLenum> texture_unit_mappings_;

		/// Active texture unit
		GLenum active_texture_unit_;

		/// Texture target mappings
		std::map<GLenum, GLuint> texture_target_mappings_;

		void init_texture_unit_mappings();

		void check_texture_unit(GLuint unit) const;

	public:
		wrapper_context(const wrapper_context &) = delete;
		wrapper_context &operator=(const wrapper_context &) = delete;

		/**
		 * @brief Throw an opengl_error if glGetError returns non-zero
		 *
		 * @throws opengl_error
		 */
		void check_errors() const;

		/**
		 * @brief Sets the flag for enabling or disabling calls to glGetError()
		 *
		 * @param enabled true if glGetError() should be called, false otherwise
		 */
		inline void checks_enabled(bool enabled)
		{ checks_enabled_ = enabled; }

		/**
		 * @brief Bind a texture to a specific unit
		 *
		 * @param unit    Texture unit index (0-based)
		 * @param texture Texture to bind to the unit
		 */
		void bind_texture_unit(GLuint unit,
							   std::optional<std::reference_wrapper<const gl::texture>> texture = std::nullopt);

		/**
		 * @brief Unbind a range of texture units
		 *
		 * Requires state tracking to be enabled.
		 *
		 * @param start First texture unit to unbind
		 * @param count Number of texture units to unbind. -1 for all remaining
		 */
		void unbind_texture_units(GLuint start, int count = -1);

		/**
		 * @brief Bind a texture to a target
		 *
		 * @param target  Texture target
		 * @param texture Texture to bind to the target
		 */
		void bind_texture(GLenum target,
						  std::optional<std::reference_wrapper<const gl::texture>> texture = std::nullopt);

		/**
		 * @brief Activate a texture unit
		 *
		 * @param unit    Texture unit index (0-based)
		 */
		void active_texture(GLuint unit);

		/**
		 * @brief Creates a new context state tracker
		 *
		 * @param state_tracking true if this context tracks state
		 * @param checks_enabled true if this context checks calls
		 */
		wrapper_context(bool state_tracking = true,
						bool checks_enabled = true);
	};

	/// Current context state tracker
	extern thread_local wrapper_context current_context;
}
}

#endif /* _SHADERTOY_GL_WRAPPER_CONTEXT_HPP_ */

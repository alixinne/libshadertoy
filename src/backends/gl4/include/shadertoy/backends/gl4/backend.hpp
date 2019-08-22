#ifndef _SHADERTOY_BACKENDS_GL4_WRAPPER_CONTEXT_HPP_
#define _SHADERTOY_BACKENDS_GL4_WRAPPER_CONTEXT_HPP_

#include "shadertoy/backends/gl4/pre.hpp"
#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/backends/gl4/texture.hpp"

#include <functional>
#include <map>
#include <optional>
#include <vector>

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
/**
 * @brief OpenGL state tracker
 */
class shadertoy_EXPORT backend : public gx::backend
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
	backend(const backend &) = delete;
	backend &operator=(const backend &) = delete;

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
	inline void checks_enabled(bool enabled) { checks_enabled_ = enabled; }

	/**
	 * @brief Bind a texture to a specific unit
	 *
	 * @param unit    Texture unit index (0-based)
	 * @param texture Texture to bind to the unit
	 */
	void bind_texture_unit(GLuint unit,
						   std::optional<std::reference_wrapper<const gl4::texture>> texture = std::nullopt);

	/**
	 * @brief Unbind a range of texture units
	 *
	 * Requires state tracking to be enabled.
	 *
	 * @param start First texture unit to unbind
	 * @param count Number of texture units to unbind. -1 for all remaining
	 */
	void unbind_texture_units(GLuint start, int count = -1) override;

	/**
	 * @brief Bind a texture to a target
	 *
	 * @param target  Texture target
	 * @param texture Texture to bind to the target
	 */
	void bind_texture(GLenum target,
					  std::optional<std::reference_wrapper<const gl4::texture>> texture = std::nullopt);

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
	backend(bool state_tracking = true, bool checks_enabled = true);

	// gx backend implementation
	// Factory methods

	std::unique_ptr<gx::buffer> make_buffer() override;

	std::unique_ptr<gx::framebuffer> make_framebuffer() override;

	std::unique_ptr<gx::program> make_program() override;

	std::unique_ptr<gx::attrib_location> make_attrib_location(GLint location) override;

	std::unique_ptr<gx::uniform_location> make_uniform_location(const gx::program &program, GLint location) override;

	std::unique_ptr<gx::query> make_query(GLenum target) override;

	std::unique_ptr<gx::renderbuffer> make_renderbuffer() override;

	std::unique_ptr<gx::sampler> make_sampler() override;

	std::unique_ptr<gx::shader> make_shader(GLenum shaderType) override;

	std::unique_ptr<gx::texture> make_texture(GLenum target) override;

	std::unique_ptr<gx::vertex_array> make_vertex_array() override;

	std::unique_ptr<gx::draw_state> make_draw_state() override;

	// Bind methods

	void bind_default_framebuffer(GLenum target) override;

	// Viewport methods

	void get_viewport(GLint viewport[4]) const override;

	void set_viewport(GLint x, GLint y, GLsizei width, GLsizei height) override;

	/**
	 * @brief Return the current context
	 */
	static backend &current();
};
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_WRAPPER_CONTEXT_HPP_ */

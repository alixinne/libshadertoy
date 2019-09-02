#ifndef _SHADERTOY_BACKENDS_GX_BACKEND_HPP_
#define _SHADERTOY_BACKENDS_GX_BACKEND_HPP_

#include "shadertoy/backends/gx/pre.hpp"

#include <memory>

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
class buffer;
class framebuffer;
class program;
class attrib_location;
class uniform_location;
class query;
class renderbuffer;
class sampler;
class shader;
class texture;
class vertex_array;
class draw_state;

/**
 * @brief Base class for backends implementations
 */
class stbackend_gx_EXPORT backend
{
	public:
	virtual ~backend();

	// Factory methods

	/**
	 * @brief Create a new buffer
	 *
	 * @param target Target for this buffer
	 */
	virtual std::unique_ptr<buffer> make_buffer(GLenum target) = 0;

	/// Create a new framebuffer
	virtual std::unique_ptr<framebuffer> make_framebuffer() = 0;

	/// Create a new program
	virtual std::unique_ptr<program> make_program() = 0;

	/**
	 * @brief Create a new attribute location
	 *
	 * @param location Location index of the attribute
	 */
	virtual std::unique_ptr<attrib_location> make_attrib_location(GLint location) = 0;

	/**
	 * @brief Create a new uniform location
	 *
	 * @param program  Program this uniform is located in
	 * @param location Location index of the uniform
	 */
	virtual std::unique_ptr<uniform_location> make_uniform_location(const program &program, GLint location) = 0;

	/**
	 * @brief Create a new query for the given target.
	 *
	 * @param target Target of the new query.
	 */
	virtual std::unique_ptr<query> make_query(GLenum target) = 0;

	/// Create a new renderbuffer
	virtual std::unique_ptr<renderbuffer> make_renderbuffer() = 0;

	/// Create a new sampler
	virtual std::unique_ptr<sampler> make_sampler() = 0;

	/**
	 * @brief Create a new shader of the given type.
	 *
	 * @param shaderType Type of the new shader
	 */
	virtual std::unique_ptr<shader> make_shader(GLenum shaderType) = 0;

	/**
	 * @brief Create a new texture for the given target.
	 *
	 * @param target Target of the new texture.
	 */
	virtual std::unique_ptr<texture> make_texture(GLenum target) = 0;

	/// Create a new vertex array
	virtual std::unique_ptr<vertex_array> make_vertex_array() = 0;

	/// Create a new draw state object
	virtual std::unique_ptr<draw_state> make_draw_state() = 0;

	// Bind methods

	/**
	 * @brief Bind the default framebuffer to the given \p target
	 *
	 * @param target Target to bind to
	 */
	virtual void bind_default_framebuffer(GLenum target) = 0;

	/**
	 * @brief Unbind a range of texture units
	 *
	 * Requires state tracking to be enabled.
	 *
	 * @param start First texture unit to unbind
	 * @param count Number of texture units to unbind. -1 for all remaining
	 */
	virtual void unbind_texture_units(GLuint start, int count = -1) = 0;

	// Viewport methods

	/**
	 * @brief Get the viewport parameters
	 *
	 * @param viewport Array to receive the result as [x, y, width, height]
	 */
	virtual void get_viewport(GLint viewport[4]) const = 0;

	/**
	 * @brief Set the viewport parameters
	 *
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 */
	virtual void set_viewport(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

namespace shadertoy
{
namespace backends
{
/**
 * @brief Get the backend used by the current thread.
 */
stbackend_gx_EXPORT const std::unique_ptr<gx::backend> &current();

/**
 * @brief Set the backend to use for the current thread.
 *
 * @param new_backend New backend instance to set
 *
 * @return Previous backend used by the thread
 */
stbackend_gx_EXPORT std::unique_ptr<gx::backend> set_current(std::unique_ptr<gx::backend> new_backend);
}
} // namespace shadertoy

#endif /* _SHADERTOY_BACKENDS_GX_BACKEND_HPP_ */

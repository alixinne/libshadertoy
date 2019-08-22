#ifndef _SHADERTOY_BACKENDS_GX_RENDERBUFFER_HPP_
#define _SHADERTOY_BACKENDS_GX_RENDERBUFFER_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Represents an OpenGL Renderbuffer object.
 */
class shadertoy_EXPORT renderbuffer : public resource
{
	public:
	/**
	 * @brief glBindRenderbuffer
	 *
	 * @param target Target to bind this renderbuffer to
	 *
	 * @throws opengl_error
	 * @throws null_renderbuffer_error
	 */
	virtual void bind(GLenum target) const = 0;

	/**
	 * @brief glBindRenderbuffer
	 *
	 * Unbinds the current renderbuffer.
	 *
	 * @param target Target to unbind the current renderbuffer from
	 *
	 * @throws opengl_error
	 */
	virtual void unbind(GLenum target) const = 0;

	/**
	 * @brief glNamedRenderbufferStorage
	 *
	 * @param internalFormat Internal format
	 * @param width          Width
	 * @param height         Height
	 *
	 * @throws opengl_error
	 * @throws null_renderbuffer_error
	 */
	virtual void storage(GLenum internalFormat, GLsizei width, GLsizei height) const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_RENDERBUFFER_HPP_ */

#ifndef _SHADERTOY_BACKENDS_GL4_RENDERBUFFER_HPP_
#define _SHADERTOY_BACKENDS_GL4_RENDERBUFFER_HPP_

#include "shadertoy/backends/gl4/pre.hpp"
#include "shadertoy/backends/gl4/resource.hpp"
#include "shadertoy/backends/gx/renderbuffer.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an attempt is made to dereference a null renderbuffer.
 */
class stbackend_gl4_EXPORT null_renderbuffer_error : public shadertoy::shadertoy_error
{
	public:
	explicit null_renderbuffer_error();
};

/**
 * @brief Represents an OpenGL Renderbuffer object.
 */
class stbackend_gl4_EXPORT renderbuffer
: public resource<renderbuffer, multi_allocator<&glCreateRenderbuffers, &glDeleteRenderbuffers>, null_renderbuffer_error, gx::renderbuffer>
{
	public:
	renderbuffer() : resource() {}
	renderbuffer(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	/**
	 * @brief glBindRenderbuffer
	 *
	 * @param target Target to bind this renderbuffer to
	 *
	 * @throws opengl_error
	 * @throws null_renderbuffer_error
	 */
	void bind(GLenum target) const override;

	/**
	 * @brief glBindRenderbuffer
	 *
	 * Unbinds the current renderbuffer.
	 *
	 * @param target Target to unbind the current renderbuffer from
	 *
	 * @throws opengl_error
	 */
	void unbind(GLenum target) const override;

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
	void storage(GLenum internalFormat, GLsizei width, GLsizei height) const override;
};
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_RENDERBUFFER_HPP_ */

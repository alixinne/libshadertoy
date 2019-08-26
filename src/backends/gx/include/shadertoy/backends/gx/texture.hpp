#ifndef _SHADERTOY_BACKENDS_GX_TEXTURE_HPP_
#define _SHADERTOY_BACKENDS_GX_TEXTURE_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

#include "shadertoy/backends/gx/buffer.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Represents an OpenGL texture.
 */
class shadertoy_EXPORT texture : public resource
{
	public:
	/**
	 * @brief glBindTexture
	 *
	 * @param target Target to bind the texture to
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void bind(GLenum target) const = 0;

	/**
	 * @brief glBindTexture
	 *
	 * Unbinds the current texture for the given \p target.
	 *
	 * @param target Target to unbind from
	 *
	 * @throws opengl_error
	 */
	virtual void unbind(GLenum target) const = 0;

	/**
	 * @brief glBindTextureUnit
	 *
	 * @param unit Unit to bind the texture to
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void bind_unit(GLuint unit) const = 0;

	/**
	 * @brief glTextureParameteri
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void parameter(GLenum pname, GLint param) const = 0;

	/**
	 * @brief glTextureParameterf
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void parameter(GLenum pname, GLfloat param) const = 0;

	/**
	 * @brief glGetTexParameteriv
	 *
	 * @param pname  Parameter name
	 * @param params Parameters
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void get_parameter(GLenum pname, GLint *params) const = 0;

	/**
	 * @brief glGetTexParameterfv
	 *
	 * @param pname  Parameter name
	 * @param params Parameters
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void get_parameter(GLenum pname, GLfloat *params) const = 0;

	/**
	 * @brief glTextureImage2DEXT
	 *
	 * @param target         Target
	 * @param level          Level
	 * @param internalFormat Internal format
	 * @param width          Width
	 * @param height         Height
	 * @param border         Border (must be 0)
	 * @param format         Format
	 * @param type           Type
	 * @param data           Data
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void image_2d(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
						  GLint border, GLenum format, GLenum type, const void *data) const = 0;

	/**
	 * @brief glGenerateTextureMipmap
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void generate_mipmap() const = 0;

#if SHADERTOY_HAS_CLEAR_TEX_IMAGE
	/**
	 * @brief glClearTexImage
	 *
	 * @param level  Level
	 * @param format Format
	 * @param type   Type
	 * @param data   Data
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void clear_tex_image(GLint level, GLenum format, GLenum type, const void *data) const = 0;
#endif

#if SHADERTOY_HAS_IMAGE_LOAD_STORE
	/**
	 * @brief glBindImageTexture
	 *
	 * @param unit    Unit
	 * @param level   Level
	 * @param layered Layered
	 * @param layer   Layer
	 * @param access  Access
	 * @param format  Format
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	virtual void bind_image(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access,
							GLenum format) const = 0;

	/**
	 * @brief glBindImageTexture without a texture to clear bindings
	 *
	 * @param unit    Unit
	 *
	 * @throws opengl_error
	 */
	virtual void unbind_image(GLuint unit) const = 0;
#endif
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_TEXTURE_HPP_ */

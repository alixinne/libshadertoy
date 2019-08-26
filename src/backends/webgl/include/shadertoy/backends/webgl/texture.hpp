#ifndef _SHADERTOY_BACKENDS_WEBGL_TEXTURE_HPP_
#define _SHADERTOY_BACKENDS_WEBGL_TEXTURE_HPP_

#include "shadertoy/backends/webgl/pre.hpp"
#include "shadertoy/backends/webgl/resource.hpp"
#include "shadertoy/backends/gx/texture.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_WEBGL_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an attempt is made to dereference a null texture.
 */
class shadertoy_EXPORT null_texture_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the null_texture_error class.
	 */
	explicit null_texture_error();
};

/**
 * @brief Represents an OpenGL texture.
 */
class shadertoy_EXPORT texture
: public resource<texture, multi_allocator<&glGenTextures, &glDeleteTextures>, null_texture_error, gx::texture>
{
	mutable GLenum target_;

	public:
	texture(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	inline GLenum target() const
	{ return target_; }

	/**
	 * @brief Create a new texture for the given target.
	 *
	 * @param target Target of the new texture.
	 *
	 * @throws opengl_error
	 */
	texture(GLenum target);

	/**
	 * @brief glBindTexture
	 *
	 * @param target Target to bind the texture to
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void bind(GLenum target) const override;

	/**
	 * @brief glBindTexture
	 *
	 * Unbinds the current texture for the given \p target.
	 *
	 * @param target Target to unbind from
	 *
	 * @throws opengl_error
	 */
	void unbind(GLenum target) const override;

	/**
	 * @brief glBindTextureUnit
	 *
	 * @param unit Unit to bind the texture to
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void bind_unit(GLuint unit) const override;

	/**
	 * @brief glTextureParameteri
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void parameter(GLenum pname, GLint param) const override;

	/**
	 * @brief glTextureParameterf
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void parameter(GLenum pname, GLfloat param) const override;

	/**
	 * @brief glGetTextureLevelParameteriv
	 *
	 * @param pname  Parameter name
	 * @param params Parameters
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void get_parameter(GLenum pname, GLint *params) const override;

	/**
	 * @brief glGetTextureLevelParameterfv
	 *
	 * @param level  Level
	 * @param pname  Parameter name
	 * @param params Parameters
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void get_parameter(GLenum pname, GLfloat *params) const override;

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
	void image_2d(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
				  GLint border, GLenum format, GLenum type, const GLvoid *data) const override;

	/**
	 * @brief glGenerateTextureMipmap
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void generate_mipmap() const override;
};
SHADERTOY_BACKENDS_WEBGL_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_WEBGL_TEXTURE_HPP_ */

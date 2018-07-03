#ifndef _SHADERTOY_GL_TEXTURE_HPP_
#define _SHADERTOY_GL_TEXTURE_HPP_

#include "shadertoy/gl/resource.hpp"

namespace shadertoy
{
namespace gl
{
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
	 * @brief Implementation of the allocator pattern for textures.
	 * Takes a target as a supplementary parameter compared to the standard
	 * resource allocators. Also links statically to glCreateTexture and
	 * glDeleteTexture according to the OpenGL spec.
	 */
	class shadertoy_EXPORT texture_allocator
	{
	public:
		/**
		 * @brief Create a new texture of the given type.
		 *
		 * @param  target Target of the texture to create
		 * @return        Id of the created texture
		 *
		 * @throws opengl_error
		 */
		GLuint create(GLenum target);

		/**
		 * @brief Delete the given texture
		 *
		 * @param resource Id of the texture to delete
		 *
		 * @throws opengl_error
		 */
		void destroy(GLuint resource);
	};

	/**
	 * @brief Represents an OpenGL texture.
	 */
	class shadertoy_EXPORT texture : public resource<texture, texture_allocator, null_texture_error>
	{
	public:
		texture(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
		resource_type &operator=(resource_type &&other) { return assign_operator(std::forward<resource_type &&>(other)); }

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
		void bind(GLenum target) const;

		/**
		 * @brief glBindTexture
		 *
		 * Unbinds the current texture for the given \p target.
		 *
		 * @param target Target to unbind from
		 *
		 * @throws opengl_error
		 */
		void unbind(GLenum target) const;

		/**
		 * @brief glBindTextureUnit
		 *
		 * @param unit Unit to bind the texture to
		 *
		 * @throws opengl_error
		 * @throws null_texture_error
		 */
		void bind_unit(GLuint unit) const;

		/**
		 * @brief glTextureParameteri
		 *
		 * @param pname Parameter name to set
		 * @param param Value of the parameter
		 *
		 * @throws opengl_error
		 * @throws null_texture_error
		 */
		void parameter(GLenum pname, GLint param) const;

		/**
		 * @brief glTextureParameterf
		 *
		 * @param pname Parameter name to set
		 * @param param Value of the parameter
		 *
		 * @throws opengl_error
		 * @throws null_texture_error
		 */
		void parameter(GLenum pname, GLfloat param) const;

		/**
		 * @brief glGetTextureLevelParameteriv
		 *
		 * @param level  Level
		 * @param pname  Parameter name
		 * @param params Parameters
		 *
		 * @throws opengl_error
		 * @throws null_texture_error
		 */
		void get_parameter(GLint level, GLenum pname, GLint *params) const;

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
		void get_parameter(GLint level, GLenum pname, GLfloat *params) const;

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
					  GLint border, GLenum format, GLenum type, const GLvoid *data) const;

		/**
		 * @brief glGenerateTextureMipmap
		 *
		 * @throws opengl_error
		 * @throws null_texture_error
		 */
		void generate_mipmap() const;

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
		void clear_tex_image(GLint level, GLenum format, GLenum type, const void *data) const;

		/**
		 * @brief glGetTextureImage
		 *
		 * @param level   Level
		 * @param format  Format
		 * @param type    Type
		 * @param bufSize Buffer size
		 * @param pixels  Buffer
		 *
		 * @throws opengl_error
		 * @throws null_texture_error
		 */
		void get_image(GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) const;
	};
}
}

#endif /* _SHADERTOY_GL_TEXTURE_HPP_ */

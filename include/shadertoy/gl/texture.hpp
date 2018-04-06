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
           * @brief Initializes a new instance of the NullTextureError class.
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
           * @brief Creates a new texture of the given type.
           *
           * @param  target Target of the texture to create
           * @return        Id of the created texture
           *
           * @throws OpenGLError
           */
          GLuint create(GLenum target);

          /**
           * @brief Deletes the given texture
           *
           * @param resource Id of the texture to delete
           *
           * @throws OpenGLError
           */
          void destroy(GLuint resource);
	};

	/**
	 * @brief Represents an OpenGL texture.
	 */
	class shadertoy_EXPORT texture : public resource<texture, texture_allocator, null_texture_error>
	{
	public:
          /**
           * @brief Creates a new texture for the given target.
           *
           * @param target Target of the new texture.
           *
           * @throws OpenGLError
           */
          texture(GLenum target);

          /**
           * @brief glBindTexture
           *
           * @param target Target to bind the texture to
           *
           * @throws OpenGLError
           * @throws NullTextureError
           */
          void bind(GLenum target);

          /**
           * @brief glTextureParameteri
           *
           * @param pname Parameter name to set
           * @param param Value of the parameter
           *
           * @throws OpenGLError
           * @throws NullTextureError
           */
          void parameter(GLenum pname, GLint param);

          /**
           * @brief glTextureParameterf
           *
           * @param pname Parameter name to set
           * @param param Value of the parameter
           *
           * @throws OpenGLError
           * @throws NullTextureError
           */
          void parameter(GLenum pname, GLfloat param);

          /**
           * @brief glGetTextureLevelParameterfv
           *
           * @param level  Level
           * @param pname  Parameter name
           * @param params Parameters
           *
           * @throws OpenGLError
           * @throws NullTextureError
           */
          void get_parameter(GLint level, GLenum pname, GLfloat *params);

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
           * @throws OpenGLError
           * @throws NullTextureError
           */
          void image_2d(GLenum target, GLint level, GLint internalFormat,
                        GLsizei width, GLsizei height, GLint border,
                        GLenum format, GLenum type, const GLvoid *data);

          /**
           * @brief glGenerateTextureMipmap
           *
           * @throws OpenGLError
           * @throws NullTextureError
           */
          void generate_mipmap();

          /**
           * @brief glClearTexImage
           *
           * @param level  Level
           * @param format Format
           * @param type   Type
           * @param data   Data
           *
           * @throws OpenGLError
           * @throws NullTextureError
           */
          void clear_tex_image(GLint level, GLenum format, GLenum type,
                               const void *data);
	};
}
}

#endif /* _SHADERTOY_GL_TEXTURE_HPP_ */

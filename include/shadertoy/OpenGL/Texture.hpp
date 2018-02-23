#ifndef _SHADERTOY_OPENGL_TEXTURE_HPP_
#define _SHADERTOY_OPENGL_TEXTURE_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null texture.
	 */
	class shadertoy_EXPORT NullTextureError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the NullTextureError class.
		 */
		explicit NullTextureError();
	};

	/**
	 * @brief Implementation of the allocator pattern for textures.
	 * Takes a target as a supplementary parameter compared to the standard
	 * resource allocators. Also links statically to glCreateTexture and
	 * glDeleteTexture according to the OpenGL spec.
	 */
	class shadertoy_EXPORT TextureAllocator
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
		GLuint Create(GLenum target);
		/**
		 * @brief Deletes the given texture
		 *
		 * @param resource Id of the texture to delete
		 *
		 * @throws OpenGLError
		 */
		void Delete(GLuint resource);
	};

	/**
	 * @brief Represents an OpenGL texture.
	 */
	class shadertoy_EXPORT Texture : public Resource<Texture, TextureAllocator, NullTextureError>
	{
	public:
		/**
		 * @brief Creates a new texture for the given target.
		 *
		 * @param target Target of the new texture.
		 *
		 * @throws OpenGLError
		 */
		Texture(GLenum target);

		/**
		 * @brief glBindTexture
		 *
		 * @param target Target to bind the texture to
		 *
		 * @throws OpenGLError
		 * @throws NullTextureError
		 */
		void Bind(GLenum target);

		/**
		 * @brief glTextureParameteri
		 *
		 * @param pname Parameter name to set
		 * @param param Value of the parameter
		 *
		 * @throws OpenGLError
		 * @throws NullTextureError
		 */
		void Parameter(GLenum pname, GLint param);

		/**
		 * @brief glTextureParameterf
		 *
		 * @param pname Parameter name to set
		 * @param param Value of the parameter
		 *
		 * @throws OpenGLError
		 * @throws NullTextureError
		 */
		void Parameter(GLenum pname, GLfloat param);

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
		void Image2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data);

		/**
		 * @brief glGenerateTextureMipmap
		 *
		 * @throws OpenGLError
		 * @throws NullTextureError
		 */
		void GenerateMipmap();
	};
}
}

#endif /* _SHADERTOY_OPENGL_TEXTURE_HPP_ */

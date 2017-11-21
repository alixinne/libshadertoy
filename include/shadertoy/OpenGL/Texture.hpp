#ifndef _SHADERTOY_OPENGL_TEXTURE_HPP_
#define _SHADERTOY_OPENGL_TEXTURE_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class NullTextureError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullTextureError();
	};

	/**
	 * Implementation of the allocator pattern for textures.
	 * Takes a target as a supplementary parameter compared to the standard
	 * resource allocators. Also links statically to glCreateTexture and
	 * glDeleteTexture according to the OpenGL spec.
	 */
	class TextureAllocator
	{
	public:
		GLuint Create(GLenum target);
		void Delete(GLuint resource);
	};

	class Texture : public Resource<Texture, TextureAllocator, NullTextureError>
	{
	public:
		/**
		 * Creates a new texture for the given target.
		 *
		 * @param target Target of the new texture.
		 */
		Texture(GLenum target);

		// glBindTexture
		void Bind(GLenum target);

		// glTextureParameteri
		void Parameter(GLenum pname, GLint param);

		// glTextureParameterf
		void Parameter(GLenum pname, GLfloat param);

		// glTextureImage2DEXT
		void Image2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data);

		// glGenerateTextureMipmap
		void GenerateMipmap();
	};
}
}

#endif /* _SHADERTOY_OPENGL_TEXTURE_HPP_ */

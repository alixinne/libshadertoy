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

	class Texture : public Resource<Texture, MultiStaticDeleter<glDeleteTextures>, NullTextureError>
	{
	public:
		// glBindTexture
		void Bind(GLenum target);
	};
}
}

#endif /* _SHADERTOY_OPENGL_TEXTURE_HPP_ */

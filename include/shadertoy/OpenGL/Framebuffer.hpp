#ifndef _SHADERTOY_OPENGL_FRAMEBUFFER_HPP_
#define _SHADERTOY_OPENGL_FRAMEBUFFER_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class NullFramebufferError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullFramebufferError();
	};

	class Texture;

	class Framebuffer : public Resource<
		Framebuffer,
		MultiAllocator<&glCreateFramebuffers, &glDeleteFramebuffers>,
		NullFramebufferError>
	{
	public:
		// glBindFramebuffer
		void Bind(GLenum target);

		// glNamedFramebufferTexture
		void Texture(GLenum attachment, Texture &texture, GLint level);
	};
}
}

#endif /* _SHADERTOY_OPENGL_FRAMEBUFFER_HPP_ */

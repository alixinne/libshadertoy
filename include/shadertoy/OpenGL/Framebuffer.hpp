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

	class Framebuffer : public Resource<Framebuffer, MultiDeleter<&glDeleteFramebuffers>, NullFramebufferError>
	{
	};
}
}

#endif /* _SHADERTOY_OPENGL_FRAMEBUFFER_HPP_ */

#ifndef _SHADERTOY_OPENGL_RENDERBUFFER_HPP_
#define _SHADERTOY_OPENGL_RENDERBUFFER_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class NullRenderbufferError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullRenderbufferError();
	};

	class Renderbuffer : public Resource<Renderbuffer, MultiDeleter<&glDeleteRenderbuffers>, NullRenderbufferError>
	{
	};
}
}

#endif /* _SHADERTOY_OPENGL_RENDERBUFFER_HPP_ */

#ifndef _SHADERTOY_OPENGL_BUFFER_HPP_
#define _SHADERTOY_OPENGL_BUFFER_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class NullBufferError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullBufferError();
	};

	class Buffer : public Resource<Buffer, MultiDeleter<&glDeleteBuffers>, NullBufferError>
	{
	};
}
}

#endif /* _SHADERTOY_OPENGL_BUFFER_HPP_ */

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

	class Buffer : public Resource<
		Buffer,
		MultiAllocator<&glCreateBuffers, &glDeleteBuffers>,
		NullBufferError>
	{
	public:
		// glBindBuffer
		void Bind(GLenum target);

		// glNamedBufferData
		void Data(GLsizei size, const void *data, GLenum usage);
	};
}
}

#endif /* _SHADERTOY_OPENGL_BUFFER_HPP_ */

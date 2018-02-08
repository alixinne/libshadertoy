#ifndef _SHADERTOY_PRE_HPP_
#define _SHADERTOY_PRE_HPP_

#include <boost/filesystem.hpp>
#include "shadertoy/shadertoy_export.h"

namespace shadertoy
{
	namespace OpenGL
	{
		class NullBufferError;
		class Buffer;

		class OpenGLError;

		class NullFramebufferError;
		class Framebuffer;

		class NullProgramError;
		class UniformLocation;
		class ProgramLinkError;
		class ProgramValidateError;
		class Program;

		class NullQueryError;
		class Query;

		class NullRenderbufferError;
		class Renderbuffer;

		class NullShaderError;
		class ShaderCompilationError;
		class ShaderAllocator;
		class Shader;

		class NullTextureError;
		class TextureAllocator;
		class Texture;
	}

	struct BufferConfig;
	struct InputConfig;
	struct ContextConfig;
	class RenderContext;
	class ShaderCompiler;
	class ShadertoyError;
	class TextureEngine;
	class ToyBuffer;
}

#endif /* _SHADERTOY_PRE_HPP_ */

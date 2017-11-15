#ifndef _SHADERTOY_PRE_HPP_
#define _SHADERTOY_PRE_HPP_

#include <boost/filesystem.hpp>
#include "shadertoy/shadertoy_export.h"

namespace shadertoy
{
	namespace OpenGL
	{
		class Framebuffer;
		class Program;
		class Renderbuffer;
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

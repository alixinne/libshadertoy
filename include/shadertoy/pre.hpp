#ifndef _SHADERTOY_PRE_HPP_
#define _SHADERTOY_PRE_HPP_

#include "shadertoy/shadertoy_export.h"

namespace shadertoy
{
	namespace gl
	{
		class null_buffer_error;
		class buffer;

		class opengl_error;

		class NullFramebufferError;
		class framebuffer;

		class null_program_error;
		class uniform_location;
		class program_link_error;
		class program_validate_error;
		class program;

		class null_query_error;
		class query;

		class NullRenderbufferError;
		class Renderbuffer;

		class null_shader_error;
		class shader_compilation_error;
		class shader_allocator;
		class shader;

		class null_texture_error;
		class texture_allocator;
		class texture;
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

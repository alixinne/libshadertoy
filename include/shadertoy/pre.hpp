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

		class null_framebuffer_error;
		class framebuffer;

		class null_program_error;
		class attrib_location;
		class uniform_location;
		class program_link_error;
		class program_validate_error;
		class program;

		class null_query_error;
		class query;

		class null_renderbuffer_error;
		class renderbuffer;

		class null_shader_error;
		class shader_compilation_error;
		class shader_allocator;
		class shader;

		class null_texture_error;
		class texture_allocator;
		class texture;
	}

	struct buffer_config;
	struct input_config;
	struct context_config;
	class render_context;
	class shader_compiler;
	class shadertoy_error;
	class texture_engine;
	class toy_buffer;
}

#endif /* _SHADERTOY_PRE_HPP_ */

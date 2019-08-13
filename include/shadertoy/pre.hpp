#ifndef _SHADERTOY_PRE_HPP_
#define _SHADERTOY_PRE_HPP_

#include "shadertoy/shadertoy_export.h"

/// Base namespace for libshadertoy
namespace shadertoy
{
	/// Definition of rendering buffers
	namespace buffers
	{
		struct buffer_output;
		class basic_buffer;
		class gl_buffer;
		class graphics_buffer;
		class program_buffer;
		class toy_buffer;
	}

	/// Shader source compiler types
	namespace compiler
	{
		class basic_part;
		class template_part;
		class file_part;

		class program_template;
	}

	/// Geometry primitives
	namespace geometry
	{
		class basic_geometry;
		class screen_quad;
	}

	/// OpenGL wrapper helpers
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

		class null_sampler_error;
		class sampler;

		class null_renderbuffer_error;
		class renderbuffer;

		class null_shader_error;
		class shader_compilation_error;
		class shader_allocator;
		class shader;

		class null_texture_error;
		class texture_allocator;
		class texture;

		class null_vertex_array_error;
		class vertex_array;
	}

	/// Buffer input class definitions
	namespace inputs
	{
		class basic_input;
		class buffer_input;
		class checker_input;
		class error_input;
		class file_input;
		class image_input;
		class noise_input;
		class openimageio_input;
		class texture_input;
	}

	/// Swap chain member class definitions
	namespace members
	{
		class basic_member;
		class buffer_member;
		class screen_member;
	}

	/// Miscellaneous utilities
	namespace utils
	{
	}

	class basic_shader_inputs;
	class bound_inputs_base;

	class swap_chain;

	class render_context;
	class shader_compiler;
	class texture_engine;
}

// Include template and utilities everywhere
#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/size.hpp"
#include "shadertoy/member_swap_policy.hpp"

#endif /* _SHADERTOY_PRE_HPP_ */

#include "shadertoy/backends/gx/draw_state.hpp"

#include <cstring>

using namespace shadertoy::backends::gx;

void draw_state::set_blend_mode(GLenum &target, GLenum new_value) const
{
	switch (new_value)
	{
	case GL_FUNC_ADD:
	case GL_FUNC_SUBTRACT:
	case GL_FUNC_REVERSE_SUBTRACT:
	case GL_MIN:
	case GL_MAX:
		target = new_value;
		break;
	default:
		throw shadertoy::shadertoy_error("Invalid blend equation mode specified");
	}
}

void draw_state::set_blend_func(GLenum &target, GLenum new_value) const
{
	switch (new_value)
	{
	case GL_ZERO:
	case GL_ONE:
	case GL_SRC_COLOR:
	case GL_ONE_MINUS_SRC_COLOR:
	case GL_DST_COLOR:
	case GL_ONE_MINUS_DST_COLOR:
	case GL_SRC_ALPHA:
	case GL_ONE_MINUS_SRC_ALPHA:
	case GL_DST_ALPHA:
	case GL_ONE_MINUS_DST_ALPHA:
	case GL_CONSTANT_COLOR:
	case GL_ONE_MINUS_CONSTANT_COLOR:
	case GL_CONSTANT_ALPHA:
	case GL_ONE_MINUS_CONSTANT_ALPHA:
	case GL_SRC_ALPHA_SATURATE:
	case GL_SRC1_COLOR:
	case GL_ONE_MINUS_SRC1_COLOR:
	case GL_SRC1_ALPHA:
	case GL_ONE_MINUS_SRC1_ALPHA:
		target = new_value;
		break;
	default:
		throw shadertoy::shadertoy_error("Invalid blend func parameter");
	}
}

draw_state::draw_state()
: enables_{ enable_default(GL_BLEND),
			enable_default(GL_COLOR_LOGIC_OP),
			enable_default(GL_CULL_FACE),
			enable_default(GL_DEBUG_OUTPUT),
			enable_default(GL_DEBUG_OUTPUT_SYNCHRONOUS),
			enable_default(GL_DEPTH_CLAMP),
			enable_default(GL_DEPTH_TEST),
			enable_default(GL_DITHER),
			enable_default(GL_FRAMEBUFFER_SRGB),
			enable_default(GL_LINE_SMOOTH),
			enable_default(GL_MULTISAMPLE),
			enable_default(GL_POLYGON_OFFSET_FILL),
			enable_default(GL_POLYGON_OFFSET_LINE),
			enable_default(GL_POLYGON_OFFSET_POINT),
			enable_default(GL_POLYGON_SMOOTH),
			enable_default(GL_PRIMITIVE_RESTART),
			enable_default(GL_PRIMITIVE_RESTART_FIXED_INDEX),
			enable_default(GL_RASTERIZER_DISCARD),
			enable_default(GL_SAMPLE_ALPHA_TO_COVERAGE),
			enable_default(GL_SAMPLE_ALPHA_TO_ONE),
			enable_default(GL_SAMPLE_COVERAGE),
			enable_default(GL_SAMPLE_SHADING),
			enable_default(GL_SAMPLE_MASK),
			enable_default(GL_SCISSOR_TEST),
			enable_default(GL_STENCIL_TEST),
			enable_default(GL_TEXTURE_CUBE_MAP_SEAMLESS),
			enable_default(GL_PROGRAM_POINT_SIZE) },
  clear_color_{ 0.f, 0.f, 0.f, 0.f }, clear_depth_(0.f), clear_stencil_(0), clear_bits_(0),
  depth_func_(GL_LESS),
#if SHADERTOY_HAS_POLYGON_MODE
  polygon_mode_(GL_FILL),
#endif
  blend_mode_rgb_(GL_FUNC_ADD),
  blend_mode_alpha_(GL_FUNC_ADD), blend_src_rgb_(GL_ONE), blend_dst_rgb_(GL_ZERO),
  blend_src_alpha_(GL_ONE), blend_dst_alpha_(GL_ZERO), blend_color_{ 0.f, 0.f, 0.f, 0.f }
#if SHADERTOY_HAS_MEMORY_BARRIER
  , memory_barrier_(0)
#endif
{
}

void draw_state::apply() const
{
	// Setup enables
	apply_enabled(GL_BLEND);
	apply_enabled(GL_COLOR_LOGIC_OP);
	apply_enabled(GL_CULL_FACE);
	apply_enabled(GL_DEBUG_OUTPUT);
	apply_enabled(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	apply_enabled(GL_DEPTH_CLAMP);
	apply_enabled(GL_DEPTH_TEST);
	apply_enabled(GL_DITHER);
	apply_enabled(GL_FRAMEBUFFER_SRGB);
	apply_enabled(GL_LINE_SMOOTH);
	apply_enabled(GL_MULTISAMPLE);
	apply_enabled(GL_POLYGON_OFFSET_FILL);
	apply_enabled(GL_POLYGON_OFFSET_LINE);
	apply_enabled(GL_POLYGON_OFFSET_POINT);
	apply_enabled(GL_POLYGON_SMOOTH);
	apply_enabled(GL_PRIMITIVE_RESTART);
	apply_enabled(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	apply_enabled(GL_RASTERIZER_DISCARD);
	apply_enabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
	apply_enabled(GL_SAMPLE_ALPHA_TO_ONE);
	apply_enabled(GL_SAMPLE_COVERAGE);
	apply_enabled(GL_SAMPLE_SHADING);
	apply_enabled(GL_SAMPLE_MASK);
	apply_enabled(GL_SCISSOR_TEST);
	apply_enabled(GL_STENCIL_TEST);
	apply_enabled(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	apply_enabled(GL_PROGRAM_POINT_SIZE);

	apply_clear_color();
	apply_clear_depth();
	apply_clear_stencil();
	apply_depth_func();
#if SHADERTOY_HAS_POLYGON_MODE
	apply_polygon_mode();
#endif
	apply_blend_mode_equation();
	apply_blend_mode_function();
	apply_blend_color();
#if SHADERTOY_HAS_MEMORY_BARRIER
	apply_memory_barrier();
#endif
}

void draw_state::clear_bits(GLbitfield new_bits)
{
	if ((new_bits & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0u)
	{
		throw shadertoy::shadertoy_error("Invalid bit combination for clear_bits");
	}

	clear_bits_ = new_bits;
}

void draw_state::depth_func(GLenum new_func)
{
	switch (new_func)
	{
	case GL_NEVER:
	case GL_ALWAYS:
	case GL_LESS:
	case GL_LEQUAL:
	case GL_GREATER:
	case GL_GEQUAL:
	case GL_EQUAL:
	case GL_NOTEQUAL:
		depth_func_ = new_func;
		break;
	default:
		throw shadertoy::shadertoy_error("Invalid depth function for depth_func");
	}
}

#if SHADERTOY_HAS_POLYGON_MODE
void draw_state::polygon_mode(GLenum new_mode)
{
	switch (new_mode)
	{
	case GL_POINT:
	case GL_LINE:
	case GL_FILL:
		polygon_mode_ = new_mode;
		break;
	default:
		throw shadertoy::shadertoy_error("Invalid polygon mode specified for polygon_mode");
	}
}
#endif

void draw_state::blend_mode_rgb(GLenum new_mode) { set_blend_mode(blend_mode_rgb_, new_mode); }

void draw_state::blend_mode_alpha(GLenum new_mode) { set_blend_mode(blend_mode_alpha_, new_mode); }

void draw_state::blend_src_rgb(GLenum new_src_rgb) { set_blend_func(blend_src_rgb_, new_src_rgb); }

void draw_state::blend_dst_rgb(GLenum new_dst_rgb) { set_blend_func(blend_dst_rgb_, new_dst_rgb); }

void draw_state::blend_src_alpha(GLenum new_src_alpha)
{
	set_blend_func(blend_src_alpha_, new_src_alpha);
}

void draw_state::blend_dst_alpha(GLenum new_dst_alpha)
{
	set_blend_func(blend_dst_alpha_, new_dst_alpha);
}

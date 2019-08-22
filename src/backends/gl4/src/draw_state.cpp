#include <epoxy/gl.h>

#include "shadertoy/backends/gl4/draw_state.hpp"

#include <cstring>
#include <iterator>

using namespace shadertoy::backends::gl4;

void draw_state::apply_enabled(GLenum cap) const
{
	// Unchecked call for performance reasons
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glIsEnabled.xhtml
	// GL_INVALID_ENUM won't be thrown by glIsEnabled nor glEnable
	bool expected = enables_[enable_idx(cap)];
	bool actual = glIsEnabled(cap);

	if (expected && !actual)
	{
		glEnable(cap);
	}
	else if (!expected && actual)
	{
		glDisable(cap);
	}
}

void draw_state::apply_clear_color() const
{
	GLfloat current_color[4];

	// Define clear parameters
	// Unchecked OpenGL calls are used when no error can be raised
	glGetFloatv(GL_COLOR_CLEAR_VALUE, std::begin(current_color));
	if (memcmp(std::begin(current_color), clear_color_.data(), sizeof(current_color)) != 0)
	{
		glClearColor(clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]);
	}
}

void draw_state::apply_clear_depth() const
{
	GLfloat current_depth;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &current_depth);
	if (current_depth != clear_depth_)
	{
		glClearDepth(clear_depth_);
	}
}

void draw_state::apply_clear_stencil() const
{
	GLint current_stencil;
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &current_stencil);
	if (current_stencil != clear_stencil_)
	{
		glClearStencil(clear_stencil_);
	}
}

void draw_state::apply_depth_func() const
{
	// Define depth testing function
	GLint current_depth_func;
	glGetIntegerv(GL_DEPTH_FUNC, &current_depth_func);
	if (static_cast<GLenum>(current_depth_func) != depth_func_)
	{
		glDepthFunc(depth_func_);
	}
}

void draw_state::apply_polygon_mode() const
{
	// Set polygon mode
	GLint current_mode;
	glGetIntegerv(GL_POLYGON_MODE, &current_mode);
	if (static_cast<GLenum>(current_mode) != polygon_mode_)
	{
		glPolygonMode(GL_FRONT_AND_BACK, polygon_mode_);
	}
}

void draw_state::apply_blend_mode_equation() const
{
	// Set blend mode equation
	GLint current_mode_rgb, current_mode_alpha;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &current_mode_rgb);
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &current_mode_alpha);
	if (static_cast<GLenum>(current_mode_rgb) != blend_mode_rgb_ ||
		static_cast<GLenum>(current_mode_alpha) != blend_mode_alpha_)
	{
		glBlendEquationSeparate(blend_mode_rgb_, blend_mode_alpha_);
	}
}

void draw_state::apply_blend_mode_function() const
{
	// Set blend mode function
	GLint current_src_rgb, current_dst_rgb, current_src_alpha, current_dst_alpha;
	glGetIntegerv(GL_BLEND_SRC_RGB, &current_src_rgb);
	glGetIntegerv(GL_BLEND_DST_RGB, &current_dst_rgb);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &current_src_alpha);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &current_dst_alpha);
	if (static_cast<GLenum>(current_src_rgb) != blend_src_rgb_ ||
		static_cast<GLenum>(current_dst_rgb) != blend_dst_rgb_ ||
		static_cast<GLenum>(current_src_alpha) != blend_src_alpha_ ||
		static_cast<GLenum>(current_dst_alpha) != blend_dst_alpha_)
	{
		glBlendFuncSeparate(blend_src_rgb_, blend_dst_rgb_, blend_src_alpha_, blend_dst_alpha_);
	}
}

void draw_state::apply_blend_color() const
{
	GLfloat current_color[4];

	// Set blend color
	glGetFloatv(GL_BLEND_COLOR, std::begin(current_color));
	if (memcmp(std::begin(current_color), blend_color_.data(), sizeof(current_color)) != 0)
	{
		glBlendColor(blend_color_[0], blend_color_[1], blend_color_[2], blend_color_[3]);
	}
}

void draw_state::apply_memory_barrier() const
{
	// Invoke memory barrier
	if (memory_barrier_)
	{
		glMemoryBarrier(memory_barrier_);
	}
}

draw_state::draw_state() {}

void draw_state::clear() const
{
	if (clear_bits_ != 0u)
	{
		glClear(clear_bits_);
	}
}

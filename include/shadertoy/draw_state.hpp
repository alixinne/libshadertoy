#ifndef _SHADERTOY_DRAW_STATE_HPP_
#define _SHADERTOY_DRAW_STATE_HPP_

#include "shadertoy/pre.hpp"

#include <array>

namespace shadertoy
{
/**
 * @brief Represents the OpenGL state that affects buffer rendering
 */
class shadertoy_EXPORT draw_state
{
	/// List of supported enables
	std::array<bool, 27> enables_;

	/// Clear color for glClear
	std::array<float, 4> clear_color_;

	/// Clear depth for glClear
	float clear_depth_;

	/// Clear stencil for glClear
	GLint clear_stencil_;

	/// Clear bits for glClear
	GLbitfield clear_bits_;

	/// Depth testing function
	GLenum depth_func_;

	/// Polygon draw mode
	GLenum polygon_mode_;

	/// Blend equation RGB mode
	GLenum blend_mode_rgb_;

	/// Blend equation alpha mode
	GLenum blend_mode_alpha_;

	/// Blend function src RGB
	GLenum blend_src_rgb_;

	/// Blend function dst RGB
	GLenum blend_dst_rgb_;

	/// Blend function src alpha
	GLenum blend_src_alpha_;

	/// Blend function dst alpha
	GLenum blend_dst_alpha_;

	/// Blend color
	std::array<float, 4> blend_color_;

	/// Memory barrier bits
	GLenum memory_barrier_;

	static constexpr size_t enable_idx(GLenum cap)
	{
		switch (cap)
		{
		case GL_BLEND:
			return 0;
		case GL_COLOR_LOGIC_OP:
			return 1;
		case GL_CULL_FACE:
			return 2;
		case GL_DEBUG_OUTPUT:
			return 3;
		case GL_DEBUG_OUTPUT_SYNCHRONOUS:
			return 4;
		case GL_DEPTH_CLAMP:
			return 5;
		case GL_DEPTH_TEST:
			return 6;
		case GL_DITHER:
			return 7;
		case GL_FRAMEBUFFER_SRGB:
			return 8;
		case GL_LINE_SMOOTH:
			return 9;
		case GL_MULTISAMPLE:
			return 10;
		case GL_POLYGON_OFFSET_FILL:
			return 11;
		case GL_POLYGON_OFFSET_LINE:
			return 12;
		case GL_POLYGON_OFFSET_POINT:
			return 13;
		case GL_POLYGON_SMOOTH:
			return 14;
		case GL_PRIMITIVE_RESTART:
			return 15;
		case GL_PRIMITIVE_RESTART_FIXED_INDEX:
			return 16;
		case GL_RASTERIZER_DISCARD:
			return 17;
		case GL_SAMPLE_ALPHA_TO_COVERAGE:
			return 18;
		case GL_SAMPLE_ALPHA_TO_ONE:
			return 19;
		case GL_SAMPLE_COVERAGE:
			return 20;
		case GL_SAMPLE_SHADING:
			return 21;
		case GL_SAMPLE_MASK:
			return 22;
		case GL_SCISSOR_TEST:
			return 23;
		case GL_STENCIL_TEST:
			return 24;
		case GL_TEXTURE_CUBE_MAP_SEAMLESS:
			return 25;
		case GL_PROGRAM_POINT_SIZE:
			return 26;
		default:
			throw shadertoy::shadertoy_error("Invalid cap in enable_idx");
		}
	}

	static constexpr bool enable_default(GLenum cap)
	{
		switch (cap)
		{
		case GL_BLEND:
			return false;
		case GL_COLOR_LOGIC_OP:
			return false;
		case GL_CULL_FACE:
			return false;
		case GL_DEBUG_OUTPUT:
			return false;
		case GL_DEBUG_OUTPUT_SYNCHRONOUS:
			return false;
		case GL_DEPTH_CLAMP:
			return false;
		case GL_DEPTH_TEST:
			return false;
		case GL_DITHER:
			return true;
		case GL_FRAMEBUFFER_SRGB:
			return false;
		case GL_LINE_SMOOTH:
			return false;
		case GL_MULTISAMPLE:
			return true;
		case GL_POLYGON_OFFSET_FILL:
			return false;
		case GL_POLYGON_OFFSET_LINE:
			return false;
		case GL_POLYGON_OFFSET_POINT:
			return false;
		case GL_POLYGON_SMOOTH:
			return false;
		case GL_PRIMITIVE_RESTART:
			return false;
		case GL_PRIMITIVE_RESTART_FIXED_INDEX:
			return false;
		case GL_RASTERIZER_DISCARD:
			return false;
		case GL_SAMPLE_ALPHA_TO_COVERAGE:
			return false;
		case GL_SAMPLE_ALPHA_TO_ONE:
			return false;
		case GL_SAMPLE_COVERAGE:
			return false;
		case GL_SAMPLE_SHADING:
			return false;
		case GL_SAMPLE_MASK:
			return false;
		case GL_SCISSOR_TEST:
			return false;
		case GL_STENCIL_TEST:
			return false;
		case GL_TEXTURE_CUBE_MAP_SEAMLESS:
			return false;
		case GL_PROGRAM_POINT_SIZE:
			return false;
		default:
			throw shadertoy::shadertoy_error("Invalid cap in enable_default");
		}
	}

	void apply_enabled(GLenum cap) const;

	void set_blend_mode(GLenum &target, GLenum new_value) const;

	void set_blend_func(GLenum &target, GLenum new_value) const;

	public:
	/**
	 * Initialize a \c draw_state with the default values according
	 * to the OpenGL specification
	 */
	draw_state();

	/**
	 * Apply the stored state to the current pipeline.
	 *
	 * This will change the state of all supported glEnable features, as well
	 * as setting the equations and parameters for blending and depth processing.
	 * Clear settings will also be applied (color, depth, and stencil.) but the
	 * clear call will not be issued.
	 */
	void apply() const;

	/**
	 * Clear the current buffers using the state clear parameters.
	 *
	 * Note that in the absence of set clear bits, this call has no effect.
	 */
	void clear() const;

	/**
	 * Enable/disable a given OpenGL feature
	 *
	 * @throw shadertoy::shadertoy_error The given capability is not a known feature of OpenGL
	 */
	inline void enable(GLenum cap, bool enabled = true) { enables_[enable_idx(cap)] = enabled; }

	/**
	 * @brief Get the current clear color for this buffer
	 *
	 * The default is (0.f, 0.f, 0.f, 0.f)
	 *
	 * @return Clear color RGBA components
	 */
	inline const std::array<float, 4> &clear_color() const { return clear_color_; }

	/**
	 * @brief Sets the clear color for this buffer
	 *
	 * @param new_color New clear color
	 */
	inline void clear_color(const std::array<float, 4> &new_color) { clear_color_ = new_color; }

	/**
	 * @brief Gets the current clear depth for this buffer
	 *
	 * The default is 0.f.
	 *
	 * @return Clear depth value
	 */
	inline float clear_depth() const { return clear_depth_; }

	/**
	 * @brief Sets the new clear depth for this buffer
	 *
	 * @param new_depth New clear depth
	 */
	inline void clear_depth(float new_depth) { clear_depth_ = new_depth; }

	/**
	 * @brief Gets the current clear stencil for this buffer
	 *
	 * The default is 0.
	 *
	 * @return Clear stencil value
	 */
	inline GLint clear_stencil() const { return clear_stencil_; }

	/**
	 * @brief Sets the new clear stencil for this buffer
	 *
	 * @param new_stencil New clear stencil
	 */
	inline void clear_stencil(GLint new_stencil) { clear_stencil_ = new_stencil; }

	/**
	 * @brief Gets the current clear bits for this buffer
	 *
	 * The default is 0.
	 *
	 * @return Enable clear bits
	 */
	inline GLbitfield clear_bits() const { return clear_bits_; }

	/**
	 * @brief Sets the new clear bits for this buffer
	 *
	 * @param new_bits New clear bits
	 *
	 * @throw shadertoy::shadertoy_error An invalid bit combination was specified
	 */
	void clear_bits(GLbitfield new_bits);

	/**
	 * @brief Gets the current depth testing function
	 *
	 * @return Depth testing function
	 */
	inline GLenum depth_func() const { return depth_func_; }

	/**
	 * @brief Sets the new depth testing function to use
	 *
	 * @param new_func New depth testing function
	 *
	 * @throw shadertoy::shadertoy_error An invalid depth testing function was specified
	 */
	void depth_func(GLenum new_func);

	/**
	 * @brief Gets the current polygon mode
	 *
	 * @return Current polygon drawing mode
	 */
	inline GLenum polygon_mode() const
	{ return polygon_mode_; }

	/**
	 * @brief Sets the new polygon mode to use
	 *
	 * @param new_mode New polygon mode
	 *
	 * @throw shadertoy::shadertoy_error An invalid polygon mode was specified
	 */
	void polygon_mode(GLenum new_mode);

	/**
	 * @brief Gets the RGB blend equation mode
	 *
	 * @return RGB blend equation mode
	 */
	inline GLenum blend_mode_rgb() const
	{ return blend_mode_rgb_; }

	/**
	 * @brief Sets the new RGB blend equation mode
	 *
	 * @param new_mode New RGB blend equation mode
	 *
	 * @throw shadertoy::shadertoy_error An invalid blend equation mode was specified
	 */
	void blend_mode_rgb(GLenum new_mode);

	/**
	 * @brief Gets the alpha blend equation mode
	 *
	 * @return alpha blend equation mode
	 */
	inline GLenum blend_mode_alpha() const
	{ return blend_mode_alpha_; }

	/**
	 * @brief Sets the new alpha blend equation mode
	 *
	 * @param new_mode New alpha blend equation mode
	 *
	 * @throw shadertoy::shadertoy_error An invalid blend equation mode was specified
	 */
	void blend_mode_alpha(GLenum new_mode);

	/**
	 * @brief Gets the srcRGB blend parameter
	 *
	 * @return srcRGB blend parameter
	 */
	inline GLenum blend_src_rgb() const
	{ return blend_src_rgb_; }

	/**
	 * @brief Sets the srcRGB blend parameter
	 *
	 * @param new_src_rgb New srcRGB blend parameter value
	 *
	 * @throw shadertoy::shadertoy_error An invalid value was specified for the parameter
	 */
	void blend_src_rgb(GLenum new_src_rgb);

	/**
	 * @brief Gets the dstRGB blend parameter
	 *
	 * @return dstRGB blend parameter
	 */
	inline GLenum blend_dst_rgb() const
	{ return blend_dst_rgb_; }

	/**
	 * @brief Sets the dstRGB blend parameter
	 *
	 * @param new_dst_rgb New dstRGB blend parameter value
	 *
	 * @throw shadertoy::shadertoy_error An invalid value was specified for the parameter
	 */
	void blend_dst_rgb(GLenum new_dst_rgb);

	/**
	 * @brief Gets the srcAlpha blend parameter
	 *
	 * @return srcAlpha blend parameter
	 */
	inline GLenum blend_src_alpha() const
	{ return blend_src_alpha_; }

	/**
	 * @brief Sets the srcAlpha blend parameter
	 *
	 * @param new_src_alpha New srcAlpha blend parameter value
	 *
	 * @throw shadertoy::shadertoy_error An invalid value was specified for the parameter
	 */
	void blend_src_alpha(GLenum new_src_alpha);

	/**
	 * @brief Gets the dstAlpha blend parameter
	 *
	 * @return dstAlpha blend parameter
	 */
	inline GLenum blend_dst_alpha() const
	{ return blend_dst_alpha_; }

	/**
	 * @brief Sets the dstAlpha blend parameter
	 *
	 * @param new_dst_alpha New dstAlpha blend parameter value
	 *
	 * @throw shadertoy::shadertoy_error An invalid value was specified for the parameter
	 */
	void blend_dst_alpha(GLenum new_dst_alpha);

	/**
	 * @brief Get the current blend color for this buffer
	 *
	 * The default is (0.f, 0.f, 0.f, 0.f)
	 *
	 * @return Clear color RGBA components
	 */
	inline const std::array<float, 4> &blend_color() const { return blend_color_; }

	/**
	 * @brief Sets the blend color for this buffer
	 *
	 * @param new_color New blend color
	 */
	inline void blend_color(const std::array<float, 4> &new_color) { blend_color_ = new_color; }

	/**
	 * @brief Get the memory barrier bits
	 *
	 * The default is none.
	 *
	 * @return Memory barrier bits
	 */
	inline const GLenum &memory_barrier() const { return memory_barrier_; }

	/**
	 * @brief Sets the memory barrier bits
	 *
	 * @param new_memory New memory barrier bits
	 */
	inline void memory_barrier(GLenum new_memory) { memory_barrier_ = new_memory & GL_ALL_BARRIER_BITS; }
};
}

#endif /* _SHADERTOY_DRAW_STATE_HPP_ */

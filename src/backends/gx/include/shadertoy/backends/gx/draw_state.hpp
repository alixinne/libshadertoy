#ifndef _SHADERTOY_BACKENDS_GX_DRAW_STATE_HPP_
#define _SHADERTOY_BACKENDS_GX_DRAW_STATE_HPP_

#include "shadertoy/backends/gx/pre.hpp"

#include "shadertoy/shadertoy_error.hpp"

#include <array>

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Represents the OpenGL state that affects buffer rendering
 */
class shadertoy_EXPORT draw_state
{
	protected:
	/// Number of enable flags
	static constexpr const size_t ENABLE_COUNT = 27;

	/// List of supported enables
	std::array<bool, ENABLE_COUNT + 1> enables_;

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

#if SHADERTOY_HAS_POLYGON_MODE
	/// Polygon draw mode
	GLenum polygon_mode_;
#endif

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

#if SHADERTOY_HAS_MEMORY_BARRIER
	/// Memory barrier bits
	GLenum memory_barrier_;
#endif

	static constexpr size_t enable_idx(GLenum cap)
	{
		switch (cap)
		{
		case GL_BLEND:
			return 0;
#if defined(GL_COLOR_LOGIC_OP)
		case GL_COLOR_LOGIC_OP:
			return 1;
#endif
		case GL_CULL_FACE:
			return 2;
#if defined(GL_DEBUG_OUTPUT)
		case GL_DEBUG_OUTPUT:
			return 3;
#endif
#if defined(GL_DEBUG_OUTPUT_SYNCHRONOUS)
		case GL_DEBUG_OUTPUT_SYNCHRONOUS:
			return 4;
#endif
#if defined(GL_DEPTH_CLAMP)
		case GL_DEPTH_CLAMP:
			return 5;
#endif
		case GL_DEPTH_TEST:
			return 6;
		case GL_DITHER:
			return 7;
#if defined(GL_FRAMEBUFFER_SRGB)
		case GL_FRAMEBUFFER_SRGB:
			return 8;
#endif
#if defined(GL_LINE_SMOOTH)
		case GL_LINE_SMOOTH:
			return 9;
#endif
#if defined(GL_MULTISAMPLE)
		case GL_MULTISAMPLE:
			return 10;
#endif
		case GL_POLYGON_OFFSET_FILL:
			return 11;
#if defined(GL_POLYGON_OFFSET_LINE)
		case GL_POLYGON_OFFSET_LINE:
			return 12;
#endif
#if defined(GL_POLYGON_OFFSET_POINT)
		case GL_POLYGON_OFFSET_POINT:
			return 13;
#endif
#if defined(GL_POLYGON_SMOOTH)
		case GL_POLYGON_SMOOTH:
			return 14;
#endif
#if defined(GL_PRIMITIVE_RESTART)
		case GL_PRIMITIVE_RESTART:
			return 15;
		case GL_PRIMITIVE_RESTART_FIXED_INDEX:
			return 16;
#endif
		case GL_RASTERIZER_DISCARD:
			return 17;
		case GL_SAMPLE_ALPHA_TO_COVERAGE:
			return 18;
#if defined(GL_SAMPLE_ALPHA_TO_ONE)
		case GL_SAMPLE_ALPHA_TO_ONE:
			return 19;
#endif
		case GL_SAMPLE_COVERAGE:
			return 20;
#if defined(GL_SAMPLE_SHADING)
		case GL_SAMPLE_SHADING:
			return 21;
#endif
#if defined(GL_SAMPLE_MASK)
		case GL_SAMPLE_MASK:
			return 22;
#endif
		case GL_SCISSOR_TEST:
			return 23;
		case GL_STENCIL_TEST:
			return 24;
#if defined(GL_TEXTURE_CUBE_MAP_SEAMLESS)
		case GL_TEXTURE_CUBE_MAP_SEAMLESS:
			return 25;
#endif
#if defined(GL_TEXTURE_PROGRAM_POINT_SIZE)
		case GL_PROGRAM_POINT_SIZE:
			return 26;
#endif
		default:
			return ENABLE_COUNT;
		}
	}

	static constexpr bool enable_default(GLenum cap)
	{
		switch (cap)
		{
		case GL_BLEND:
			return false;
#if defined(GL_COLOR_LOGIC_OP)
		case GL_COLOR_LOGIC_OP:
			return false;
#endif
		case GL_CULL_FACE:
			return false;
#if defined(GL_DEBUG_OUTPUT)
		case GL_DEBUG_OUTPUT:
			return false;
#endif
#if defined(GL_DEBUG_OUTPUT_SYNCHRONOUS)
		case GL_DEBUG_OUTPUT_SYNCHRONOUS:
			return false;
#endif
#if defined(GL_DEPTH_CLAMP)
		case GL_DEPTH_CLAMP:
			return false;
#endif
		case GL_DEPTH_TEST:
			return false;
		case GL_DITHER:
			return true;
#if defined(GL_FRAMEBUFFER_SRGB)
		case GL_FRAMEBUFFER_SRGB:
			return false;
#endif
#if defined(GL_LINE_SMOOTH)
		case GL_LINE_SMOOTH:
			return false;
#endif
#if defined(GL_MULTISAMPLE)
		case GL_MULTISAMPLE:
			return true;
#endif
		case GL_POLYGON_OFFSET_FILL:
			return false;
#if defined(GL_POLYGON_OFFSET_LINE)
		case GL_POLYGON_OFFSET_LINE:
			return false;
#endif
#if defined(GL_POLYGON_OFFSET_POINT)
		case GL_POLYGON_OFFSET_POINT:
			return false;
#endif
#if defined(GL_POLYGON_SMOOTH)
		case GL_POLYGON_SMOOTH:
			return false;
#endif
#if defined(GL_PRIMITIVE_RESTART)
		case GL_PRIMITIVE_RESTART:
			return false;
#endif
		case GL_PRIMITIVE_RESTART_FIXED_INDEX:
			return false;
		case GL_RASTERIZER_DISCARD:
			return false;
		case GL_SAMPLE_ALPHA_TO_COVERAGE:
			return false;
#if defined(GL_SAMPLE_ALPHA_TO_ONE)
		case GL_SAMPLE_ALPHA_TO_ONE:
			return false;
#endif
		case GL_SAMPLE_COVERAGE:
			return false;
#if defined(GL_SAMPLE_SHADING)
		case GL_SAMPLE_SHADING:
			return false;
#endif
#if defined(GL_SAMPLE_MASK)
		case GL_SAMPLE_MASK:
			return false;
#endif
		case GL_SCISSOR_TEST:
			return false;
		case GL_STENCIL_TEST:
			return false;
#if defined(GL_TEXTURE_CUBE_MAP_SEAMLESS)
		case GL_TEXTURE_CUBE_MAP_SEAMLESS:
			return false;
#endif
#if defined(GL_PROGRAM_POINT_SIZE)
		case GL_PROGRAM_POINT_SIZE:
			return false;
#endif
		default:
			return false;
		}
	}

	virtual void apply_enabled(GLenum cap) const = 0;

	virtual void apply_clear_color() const = 0;

	virtual void apply_clear_depth() const = 0;

	virtual void apply_clear_stencil() const = 0;

	virtual void apply_depth_func() const = 0;

#if SHADERTOY_HAS_POLYGON_MODE
	virtual void apply_polygon_mode() const = 0;
#endif

	virtual void apply_blend_mode_equation() const = 0;

	virtual void apply_blend_mode_function() const = 0;

	virtual void apply_blend_color() const = 0;

#if SHADERTOY_HAS_MEMORY_BARRIER
	virtual void apply_memory_barrier() const = 0;
#endif

	void set_blend_mode(GLenum &target, GLenum new_value) const;

	void set_blend_func(GLenum &target, GLenum new_value) const;

	/**
	 * Initialize a \c draw_state with the default values according
	 * to the OpenGL specification
	 */
	draw_state();

	public:
	virtual ~draw_state() = default;

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
	virtual void clear() const = 0;

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

#if SHADERTOY_HAS_POLYGON_MODE
	/**
	 * @brief Gets the current polygon mode
	 *
	 * @return Current polygon drawing mode
	 */
	inline GLenum polygon_mode() const { return polygon_mode_; }

	/**
	 * @brief Sets the new polygon mode to use
	 *
	 * @param new_mode New polygon mode
	 *
	 * @throw shadertoy::shadertoy_error An invalid polygon mode was specified
	 */
	void polygon_mode(GLenum new_mode);
#endif

	/**
	 * @brief Gets the RGB blend equation mode
	 *
	 * @return RGB blend equation mode
	 */
	inline GLenum blend_mode_rgb() const { return blend_mode_rgb_; }

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
	inline GLenum blend_mode_alpha() const { return blend_mode_alpha_; }

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
	inline GLenum blend_src_rgb() const { return blend_src_rgb_; }

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
	inline GLenum blend_dst_rgb() const { return blend_dst_rgb_; }

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
	inline GLenum blend_src_alpha() const { return blend_src_alpha_; }

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
	inline GLenum blend_dst_alpha() const { return blend_dst_alpha_; }

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

#if SHADERTOY_HAS_MEMORY_BARRIER
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
	inline void memory_barrier(GLenum new_memory)
	{
		memory_barrier_ = new_memory & GL_ALL_BARRIER_BITS;
	}
#endif
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_DRAW_STATE_HPP_ */

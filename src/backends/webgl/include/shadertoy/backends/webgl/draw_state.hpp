
#ifndef _SHADERTOY_BACKENDS_WEBGL_DRAW_STATE_HPP_
#define _SHADERTOY_BACKENDS_WEBGL_DRAW_STATE_HPP_

#include "shadertoy/backends/webgl/pre.hpp"

#include "shadertoy/backends/gx/draw_state.hpp"

SHADERTOY_BACKENDS_WEBGL_NAMESPACE_BEGIN
/**
 * @brief Represents the OpenGL state that affects buffer rendering
 */
class shadertoy_EXPORT draw_state : public gx::draw_state
{
	protected:
	void apply_enabled(GLenum cap) const override;

	void apply_clear_color() const override;

	void apply_clear_depth() const override;

	void apply_clear_stencil() const override;

	void apply_depth_func() const override;

	void apply_blend_mode_equation() const override;

	void apply_blend_mode_function() const override;

	void apply_blend_color() const override;

	public:
	/**
	 * Initialize a \c draw_state with the default values according
	 * to the OpenGL specification
	 */
	draw_state();

	/**
	 * Clear the current buffers using the state clear parameters.
	 *
	 * Note that in the absence of set clear bits, this call has no effect.
	 */
	void clear() const override;
};
SHADERTOY_BACKENDS_WEBGL_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_WEBGL_DRAW_STATE_HPP_ */

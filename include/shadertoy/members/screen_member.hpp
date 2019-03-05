#ifndef _SHADERTOY_MEMBERS_SCREEN_MEMBER_HPP_
#define _SHADERTOY_MEMBERS_SCREEN_MEMBER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/members/basic_member.hpp"

#include "shadertoy/draw_state.hpp"

namespace shadertoy
{
namespace members
{

/**
 * @brief swap_chain member that renders to the default framebuffer
 *
 * The screen_member can be added to a swap_chain to render the output of any
 * basic_member (which may not be part of the same swap chain) to the default
 * framebuffer of the current OpenGL context.
 *
 * The default constructor (screen_member#screen_member(rsize_ref &&)) queries
 * the swap_chain object in order to find out the latest rendered member. Note
 * that this fails if the swap chain only contains the screen_member instance.
 *
 * A member (screen_member#screen_member(rsize_ref &&, std::shared_ptr<members::basic_member>))
 * can instead be specified. The screen_member instance then renders the output
 * of the specified member independently of the preceding node in the
 * swap_chain.
 *
 * The screen member also holds a draw_state member which controls how the
 * final texture is drawn on screen. This allows setting blending options and
 * clear colors for the rendering target.
 *
 * Additional parameters are available to change the viewport when rendering.
 */
class shadertoy_EXPORT screen_member : public basic_member
{
	/// Member to render the output of
	std::weak_ptr<basic_member> member_;

	/// Sampler object to control how the texture is rendered to the screen
	gl::sampler sampler_;

	/// Viewport X
	int viewport_x_;
	
	/// Viewport Y
	int viewport_y_;

	/// Size reference for the viewport call
	rsize_ref viewport_size_;

	/// OpenGL drawing state
	draw_state state_;

protected:
	/**
	 * @brief Implement rendering the last swap chain output to the screen
	 *
	 * @param chain   Current swap_chain being rendered
	 * @param context Context to use for rendering
	 */
	void render_member(const swap_chain &chain, const render_context &context) override;

	/** @cond UNUSED */
	void init_member(const swap_chain &chain, const render_context &context) override;

	void allocate_member(const swap_chain &chain, const render_context &context) override;
	/** @endcond */

	/**
	 * @brief Return the associated output or the latest output in the swap chain
	 *
	 * @param chain Swap chain to pull the output from
	 *
	 * @return Pointer to the OpenGL texture output object
	 */
	gl::texture *output(const swap_chain &chain);

public:
	/**
	 * @brief Initialize a new instance of the screen_member class
	 *
	 * By default, this instance will render the last rendered output in the
	 * current swap chain.
	 *
	 * @param viewport_size Initial viewport size
	 */
	screen_member(rsize_ref &&viewport_size);

	/**
	 * @brief Initialize a new instance of the screen_member class
	 *
	 * By default, this instance will render the last rendered output in the
	 * current swap chain.
	 *
	 * @param viewport_x    Initial viewport X offset
	 * @param viewport_y    Initial viewport Y offset
	 * @param viewport_size Initial viewport size
	 */
	screen_member(int viewport_x, int viewport_y, rsize_ref &&viewport_size);

	/**
	 * @brief Initialize a new instance of the screen_member class
	 *
	 * By default, this instance will render the associated output instead
	 * of the last rendered output in the current swap chain.
	 *
	 * @param viewport_size Initial viewport size
	 * @param member        Target member
	 */
	screen_member(rsize_ref &&viewport_size, std::weak_ptr<members::basic_member> member);


	/**
	 * @brief Initialize a new instance of the screen_member class
	 *
	 * By default, this instance will render the associated output instead
	 * of the last rendered output in the current swap chain.
	 *
	 * @param viewport_x    Initial viewport X offset
	 * @param viewport_y    Initial viewport Y offset
	 * @param viewport_size Initial viewport size
	 * @param member        Target member
	 */
	screen_member(int viewport_x, int viewport_y, rsize_ref &&viewport_size, std::weak_ptr<members::basic_member> member);

	/**
	 * @brief Return the same output that will be drawn to the screen
	 *
	 * Note that unless an explicit output has been associated to this member,
	 * this method will return null since in the absence of a swap chain, the
	 * "latest" output is not defined.
	 *
	 * @return Pointer to the corresponding OpenGL texture object. The pointer
	 * is valid for the lifetime of the basic_member this screen_member is
	 * rendering to the screen. See basic_member#output for more details.
	 */
	gl::texture *output() override;

	/**
	 * @brief Obtain a reference to the sampler object of this member
	 *
	 * @return Reference to the OpenGL sampler object used for the rendering
	 */
	inline const gl::sampler &sampler() const
	{ return sampler_; }

	/**
	 * @brief Obtain the viewport X offset
	 *
	 * @return Value of the viewport X offset
	 */
	inline int viewport_x() const
	{ return viewport_x_; }

	/**
	 * @brief Set the viewport X offset
	 *
	 * @param new_viewport_x New viewport X offset
	 */
	inline void viewport_x(int new_viewport_x)
	{ viewport_x_ = new_viewport_x; }

	/**
	 * @brief Obtain the viewport Y offset
	 *
	 * @return Value of the viewport Y offset
	 */
	inline int viewport_y() const
	{ return viewport_y_; }

	/**
	 * @brief Set the viewport Y offset
	 *
	 * @param new_viewport_y New viewport Y offset
	 */
	inline void viewport_y(int new_viewport_y)
	{ viewport_y_ = new_viewport_y; }

	/**
	 * @brief Obtain the viewport size object
	 *
	 * @return Reference to the viewport size object
	 */
	inline const rsize_ref &viewport_size() const
	{ return viewport_size_; }

	/**
	 * @brief Set the viewport size object
	 *
	 * @param new_viewport_size New viewport size
	 */
	inline void viewport_size(rsize_ref &&new_viewport_size)
	{ viewport_size_ = std::move(new_viewport_size); }

	/**
	 * @brief Get a reference to the OpenGL state
	 *
	 * @return Reference to the OpenGL state
	 */
	inline const draw_state &state() const
	{ return state_; }

	/**
	 * @brief Get a reference to the OpenGL state
	 *
	 * @return Reference to the OpenGL state
	 */
	inline draw_state &state()
	{ return state_; }
};

/**
 * @brief Construct a pointer to a screen member
 *
 * @param args Arguments to screen_member#screen_member
 *
 * @return Pointer to the constructed screen_member
 *
 * @see screen_member#screen_member
 */
template<typename... Args>
std::shared_ptr<screen_member> make_screen(Args&&... args)
{
	return std::make_shared<screen_member>(std::forward<Args>(args)...);
}
}
}

#endif /* _SHADERTOY_MEMBERS_SCREEN_MEMBER_HPP_ */

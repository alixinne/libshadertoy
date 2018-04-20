#ifndef _SHADERTOY_MEMBERS_SCREEN_MEMBER_HPP_
#define _SHADERTOY_MEMBERS_SCREEN_MEMBER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/members/basic_member.hpp"

namespace shadertoy
{
namespace members
{

class shadertoy_EXPORT screen_member : public basic_member
{
	/// Sampler object to control how the texture is rendered to the screen
	gl::sampler sampler_;

	/// Viewport X
	int viewport_x_;
	
	/// Viewport Y
	int viewport_y_;

	/// Size reference for the viewport call
	rsize_ref viewport_size_;

protected:
	/**
	 * @brief Implements rendering the last swap chain output to the screen
	 *
	 * @param chain   Current swap_chain being rendered
	 * @param context Context to use for rendering
	 */
	void render_member(swap_chain &chain, render_context &context) override;

	/**
	 * @brief unused
	 */
	void init_member(swap_chain &chain, render_context &context) override;

	/**
	 * @brief unused
	 */
	void allocate_member(swap_chain &chain, render_context &context) override;

public:
	/**
	 * @brief Initializes a new instance of the screen_member class
	 */
	screen_member();

	/**
	 * @brief Returns the same output that will be drawn to the screen
	 *
	 * @param chain  Chain to consider for the output
	 */
	std::shared_ptr<gl::texture> output(swap_chain &chain) override;

	/**
	 * @brief Obtains a reference to the sampler object of this member
	 */
	inline const gl::sampler &sampler() const
	{ return sampler_; }

	/**
	 * @brief Obtains the viewport X offset
	 */
	inline int viewport_x() const
	{ return viewport_x_; }

	/**
	 * @brief Sets the viewport X offset
	 */
	inline void viewport_x(int new_viewport_x)
	{ viewport_x_ = new_viewport_x; }

	/**
	 * @brief Obtains the viewport Y offset
	 */
	inline int viewport_y() const
	{ return viewport_y_; }

	/**
	 * @brief Sets the viewport Y offset
	 */
	inline void viewport_y(int new_viewport_y)
	{ viewport_y_ = new_viewport_y; }

	/**
	 * @brief Obtains the viewport size object
	 */
	inline const rsize_ref &viewport_size() const
	{ return viewport_size_; }

	/**
	 * @brief Obtains the viewport size object
	 */
	inline rsize_ref &viewport_size()
	{ return viewport_size_; }

	/**
	 * @brief Sets the viewport size object
	 */
	inline void viewport_size(rsize_ref new_viewport_size)
	{ viewport_size_ = new_viewport_size; }
};
}
}

#endif /* _SHADERTOY_MEMBERS_SCREEN_MEMBER_HPP_ */

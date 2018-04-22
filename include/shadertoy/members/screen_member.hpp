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
	/// Member to render the output of
	std::shared_ptr<basic_member> member_;

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
	void render_member(const swap_chain &chain, const render_context &context) override;

	/**
	 * @brief unused
	 */
	void init_member(const swap_chain &chain, const render_context &context) override;

	/**
	 * @brief unused
	 */
	void allocate_member(const swap_chain &chain, const render_context &context) override;

	/// Return the associated output or the latest output in the swap chain
	std::shared_ptr<gl::texture> output(const swap_chain &chain);

public:
	/**
	 * @brief Initializes a new instance of the screen_member class
	 *
	 * By default, this instance will render the last rendered output in the
	 * current swap chain.
	 *
	 * @param viewport_size Initial viewport size
	 */
	screen_member(rsize_ref &&viewport_size);

	/**
	 * @brief Initializes a new instance of the screen_member class
	 *
	 * By default, this instance will render the associated output instead
	 * of the last rendered output in the current swap chain.
	 *
	 * @param viewport_size Initial viewport size
	 * @param member        Target member
	 */
	screen_member(rsize_ref &&viewport_size, std::shared_ptr<members::basic_member> member);

	/**
	 * @brief Returns the same output that will be drawn to the screen
	 *
	 * Note that unless an explicit output has been associated to this member,
	 * this method will return null since in the absence of a swap chain, the
	 * "latest" output is not defined.
	 */
	std::shared_ptr<gl::texture> output() override;

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
	 * @brief Sets the viewport size object
	 */
	inline void viewport_size(rsize_ref &&new_viewport_size)
	{ viewport_size_ = std::move(new_viewport_size); }
};

template<typename... Args>
std::shared_ptr<screen_member> make_screen(Args&&... args)
{
	return std::make_shared<screen_member>(std::forward<Args>(args)...);
}

}
}

#endif /* _SHADERTOY_MEMBERS_SCREEN_MEMBER_HPP_ */

#ifndef _SHADERTOY_MEMBERS_BASIC_MEMBER_HPP_
#define _SHADERTOY_MEMBERS_BASIC_MEMBER_HPP_

#include "shadertoy/pre.hpp"

#include <memory>

namespace shadertoy
{
namespace members
{

/**
 * @brief Base class for swap_chain members
 */
class shadertoy_EXPORT basic_member
{
protected:
	/**
	 * @brief Must be implemented by derived classes to perform
	 * the render step for this member
	 *
	 * @param chain   Current swap_chain being rendered
	 * @param context Context to use for rendering
	 */
	virtual void render_member(const swap_chain &chain, const render_context &context) = 0;

	/**
	 * @brief Must be implemented by derived classes to perform
	 * initialization
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for initialization
	 */
	virtual void init_member(const swap_chain &chain, const render_context &context) = 0;

	/**
	 * @brief Must be implemented by derived classes to perform
	 * texture allocation on size changes
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for texture allocation
	 */
	virtual void allocate_member(const swap_chain &chain, const render_context &context) = 0;

public:
	/**
	 * @brief Renders this member
	 *
	 * @param chain   Current swap_chain being rendered
	 * @param context Context to use for rendering
	 */
	void render(const swap_chain &chain, const render_context &context);

	/**
	 * @brief Initialize this member
	 *
	 * @param chain   Current swap_chain being initialized
	 * @param context Context to use for initialization
	 */
	void init(const swap_chain &chain, const render_context &context);

	/**
	 * @brief Allocate the textures for this member
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for texture allocation
	 */
	void allocate(const swap_chain &chain, const render_context &context);

	/**
	 * @brief Obtains the output of this member
	 *
	 * @return Output of this member, or null if this member has no output
	 */
	virtual std::shared_ptr<gl::texture> output() = 0;
};
}
}

#endif /* _SHADERTOY_MEMBERS_BASIC_MEMBER_HPP_ */

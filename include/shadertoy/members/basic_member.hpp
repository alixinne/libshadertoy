#ifndef _SHADERTOY_MEMBERS_BASIC_MEMBER_HPP_
#define _SHADERTOY_MEMBERS_BASIC_MEMBER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/output_name.hpp"

#include <memory>
#include <tuple>
#include <vector>

namespace shadertoy
{
namespace members
{

/**
 * @brief Named output of a member
 *
 * This type holds the naming details of a given output in a render pass as
 * well as a pointer to the texture holding the result of the pass.
 */
typedef std::tuple<output_name_info_t, gl::texture *> member_output_t;

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
	 * @brief Render this member
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
	 * @brief Obtain the output of this member
	 *
	 * @return Output of this member, or empty vector if this member has no
	 * outputs. The returned pointers (if not null) are guaranteed to be valid
	 * as long as the current basic_member and the owner of the returned exist.
	 * This should be the case as long as no calls to state-changing methods
	 * such as basic_member#allocate and basic_member#init are made, nor
	 * renders (basic_member#render).
	 *
	 * @see shadertoy::members::member_output_t
	 */
	virtual std::vector<member_output_t> output() = 0;

	/**
	 * @brief Obtain the index in the output vector of a given location
	 *
	 * @param name Output name to find
	 *
	 * @return -1 if the location was not found, otherwise returns the index of the output
	 *
	 * @see shadertoy::output_name_t
	 */
	virtual int find_output(const output_name_t &name) const;
};
}
}

#endif /* _SHADERTOY_MEMBERS_BASIC_MEMBER_HPP_ */

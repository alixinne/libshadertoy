#ifndef _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_
#define _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/members/basic_member.hpp"

namespace shadertoy
{
namespace members
{

/**
 * @brief Represents a swap chain member backed by a buffer
 */
class shadertoy_EXPORT buffer_member : public basic_member
{
	/// Buffer referenced by this member
	std::shared_ptr<buffers::basic_buffer> buffer_;

protected:
	/**
	 * @brief Renders the buffer using the given \p context
	 *
	 * @param chain   Current swap_chain being rendered
	 * @param context Context to use for rendering
	 */
	void render_member(swap_chain &chain, render_context &context) override;

	/**
	 * @brief Initializes the associated buffer
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for initialization
	 */
	void init_member(swap_chain &chain, render_context &context) override;

	/**
	 * @brief Allocates the associated buffer's textures
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for texture allocation
	 */
	void allocate_member(swap_chain &chain, render_context &context) override;

public:
	/**
	 * @brief Initializes a new buffer swap chain member
	 *
	 * @param buffer Associated buffer
	 */
	buffer_member(std::shared_ptr<buffers::basic_buffer> buffer);

	/**
	 * @brief Get the buffer associated with this member
	 *
	 * @return Pointer to the buffer associated with this member
	 */
	inline std::shared_ptr<buffers::basic_buffer> buffer() const
	{ return buffer_; }

	/**
	 * @brief Returns the buffer's latest output in the current chain
	 *
	 * @param chain  Chain to consider for the output
	 */
	std::shared_ptr<gl::texture> output(swap_chain &chain) override;
};
}
}

#endif /* _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_ */

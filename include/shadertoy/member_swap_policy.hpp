#ifndef _SHADERTOY_MEMBER_SWAP_POLICY_HPP_
#define _SHADERTOY_MEMBER_SWAP_POLICY_HPP_

namespace shadertoy
{

/**
 * @brief Represents the texture swapping policy in a swap chain
 */
enum class member_swap_policy
{
	/**
	 * @brief Rendering the target member will always be done in the same texture.
	 *
	 * This is suited for buffers that only process inputs from other passes and
	 * which do not store state by rendering and reading their own output. Note
	 * that there might be a performance hit if the target pass is on the
	 * critical path.
	 */
	single_buffer,
	/**
	 * @brief Rendering the target member will be done in one of two alternating textures.
	 *
	 * This is commonly known as ping-pong, which requires twice the memory but
	 * allows storing state in a single pass. It might also improve performance
	 * if the target pass is on the critical path and pipeline passes are
	 * required.
	 */
	double_buffer
};
}

#endif /* _SHADERTOY_MEMBER_SWAP_POLICY_HPP_ */

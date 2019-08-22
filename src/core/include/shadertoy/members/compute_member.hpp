#ifndef _SHADERTOY_MEMBERS_COMPUTE_MEMBER_HPP_
#define _SHADERTOY_MEMBERS_COMPUTE_MEMBER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/members/basic_member.hpp"

#include "shadertoy/buffers/compute_buffer.hpp"

#include <memory>

namespace shadertoy
{
namespace members
{

/**
 * @brief Represents a swap chain member backed by a buffer
 *
 * Here, `buffer` refers to a compute_buffer. Management of
 * resources is done manually by the user of the library.
 */
class shadertoy_EXPORT compute_member : public basic_member
{
	/// Buffer referenced by this member
	std::shared_ptr<buffers::compute_buffer> buffer_;

	protected:
	/**
	 * @brief Render the buffer using the given \p context
	 *
	 * @param chain   Current swap_chain being rendered
	 * @param context Context to use for rendering
	 */
	void render_member(const swap_chain &chain, const render_context &context) override;

	/**
	 * @brief Initialize the associated buffer
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for initialization
	 */
	void init_member(const swap_chain &chain, const render_context &context) override;

	/**
	 * @brief Allocate the associated buffer's textures
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for texture allocation
	 */
	void allocate_member(const swap_chain &chain, const render_context &context) override;

	public:
	/**
	 * @brief Initialize a new buffer swap chain member
	 *
	 * @param buffer          Associated buffer
	 */
	compute_member(std::shared_ptr<buffers::compute_buffer> buffer);

	virtual ~compute_member() = default;

	/**
	 * @brief Get the buffer associated with this member
	 *
	 * @return Pointer to the buffer associated with this member
	 */
	inline const std::shared_ptr<buffers::compute_buffer> &buffer() const { return buffer_; }

	/**
	 * @brief Return the buffer's latest output in the current chain
	 *
	 * @return Texture objects representing the output of the buffer
	 */
	std::vector<member_output_t> output() override;

	/**
	 * @brief Obtain the index in the output vector of a given location
	 *
	 * @param name Output name to find
	 *
	 * @return -1 if the location was not found, otherwise returns the index of the output
	 *
	 * @see shadertoy::output_name_t
	 */
	int find_output(const output_name_t &name) const override;
};

/**
 * @brief Construct a pointer to a compute_member
 *
 * @param args Arguments to compute_member#compute_member
 *
 * @return Pointer to the constructed compute_member
 *
 * @see compute_member#compute_member
 */
template <typename... Args> std::shared_ptr<compute_member> make_compute(Args &&... args)
{
	return std::make_shared<compute_member>(std::forward<Args>(args)...);
}

/**
 * @brief Construct a pointer to a compute_member
 *
 * @param chain           Swap chain this member is being constructed from. The
 *                        non-specified parameters will get their defaults from this chain.
 * @param buffer          Buffer to add to the swap chain
 *
 * @return Pointer to the constructed compute_member
 *
 * @see compute_member#compute_member(std::shared_ptr<buffers::compute_buffer>, rsize_ref &&, GLint, member_swap_policy)
 */
std::shared_ptr<compute_member>
shadertoy_EXPORT make_member(const swap_chain &chain, std::shared_ptr<buffers::compute_buffer> buffer);
} // namespace members
} // namespace shadertoy

#endif /* _SHADERTOY_MEMBERS_COMPUTE_MEMBER_HPP_ */

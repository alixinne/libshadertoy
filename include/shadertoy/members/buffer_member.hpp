#ifndef _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_
#define _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/members/basic_member.hpp"

#include "shadertoy/io_resource.hpp"

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

	/// IO resource object that handles texture allocations
	io_resource io_;

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
	 * @param render_size     Initial render size
	 * @param internal_format Internal format of the rendering textures
	 * @param swap_policy     Texture swapping policy for the rendering textures
	 */
	buffer_member(std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size, GLint internal_format, member_swap_policy swap_policy);

	/**
	 * @brief Get the buffer associated with this member
	 *
	 * @return Pointer to the buffer associated with this member
	 */
	inline const std::shared_ptr<buffers::basic_buffer> &buffer() const
	{ return buffer_; }

	/**
	 * @brief Get the IO resource object that holds this member's textures
	 * 
	 * @return Reference to the IO resource object
	 */
	inline const io_resource &io() const
	{ return io_; }

	/**
	 * @brief Return the buffer's latest output in the current chain
	 *
	 * @return Texture object representing the output of the buffer
	 */
	gl::texture *output() override;
};

/**
 * @brief Construct a pointer to a buffer_member
 *
 * @param args Arguments to buffer_member#buffer_member
 *
 * @return Pointer to the constructed buffer_member
 *
 * @see buffer_member#buffer_member
 */
template<typename... Args>
std::shared_ptr<buffer_member> make_buffer(Args&&... args)
{
	return std::make_shared<buffer_member>(std::forward<Args>(args)...);
}

/**
 * @brief Construct a pointer to a buffer_member
 *
 * @param chain           Swap chain this member is being constructed from. The
 *                        non-specified parameters will get their defaults from this chain.
 * @param buffer          Buffer to add to the swap chain
 * @param render_size     Size to render the buffer at when integrated in the swap chain
 *
 * @return Pointer to the constructed buffer_member
 *
 * @see buffer_member#buffer_member(std::shared_ptr<buffers::basic_buffer>, rsize_ref &&, GLint, member_swap_policy)
 */
std::shared_ptr<buffer_member> make_member(const swap_chain &chain, std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size);

/**
 * @brief Construct a pointer to a buffer_member
 *
 * @param chain           Swap chain this member is being constructed from. The
 *                        non-specified parameters will get their defaults from this chain.
 * @param buffer          Buffer to add to the swap chain
 * @param render_size     Size to render the buffer at when integrated in the swap chain
 * @param internal_format Internal format of the rendering textures for this member
 *
 * @return Pointer to the constructed buffer_member
 *
 * @see buffer_member#buffer_member(std::shared_ptr<buffers::basic_buffer>, rsize_ref &&, GLint, member_swap_policy)
 */
std::shared_ptr<buffer_member> make_member(const swap_chain &chain, std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size, GLint internal_format);

/**
 * @brief Construct a pointer to a buffer_member
 *
 * @param chain           Swap chain this member is being constructed from. The
 *                        non-specified parameters will get their defaults from this chain.
 * @param buffer          Buffer to add to the swap chain
 * @param render_size     Size to render the buffer at when integrated in the swap chain
 * @param internal_format Internal format of the rendering textures for this member
 * @param swap_policy     Texture swapping policy for this member
 *
 * @return Pointer to the constructed buffer_member
 *
 * @see buffer_member#buffer_member(std::shared_ptr<buffers::basic_buffer>, rsize_ref &&, GLint, member_swap_policy)
 */
std::shared_ptr<buffer_member> make_member(const swap_chain &chain, std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size, GLint internal_format, member_swap_policy swap_policy);
}
}

#endif /* _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_ */

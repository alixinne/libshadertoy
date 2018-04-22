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
	 * @brief Renders the buffer using the given \p context
	 *
	 * @param chain   Current swap_chain being rendered
	 * @param context Context to use for rendering
	 */
	void render_member(const swap_chain &chain, const render_context &context) override;

	/**
	 * @brief Initializes the associated buffer
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for initialization
	 */
	void init_member(const swap_chain &chain, const render_context &context) override;

	/**
	 * @brief Allocates the associated buffer's textures
	 *
	 * @param chain   Current swap_chain
	 * @param context Context to use for texture allocation
	 */
	void allocate_member(const swap_chain &chain, const render_context &context) override;

public:
	/**
	 * @brief Initializes a new buffer swap chain member
	 *
	 * @param buffer      Associated buffer
	 * @param render_size Initial render size
	 */
	buffer_member(std::shared_ptr<buffers::basic_buffer> buffer, rsize_ref &&render_size);

	/**
	 * @brief Get the buffer associated with this member
	 *
	 * @return Pointer to the buffer associated with this member
	 */
	inline std::shared_ptr<buffers::basic_buffer> buffer() const
	{ return buffer_; }

	/**
	 * @brief Get the IO resource object that holds this member's textures
	 * 
	 * @return Reference to the IO resource object
	 */
	inline const io_resource &io() const
	{ return io_; }

	/**
	 * @brief Returns the buffer's latest output in the current chain
	 */
	std::shared_ptr<gl::texture> output() override;
};

std::shared_ptr<buffer_member> member_data(std::shared_ptr<buffers::basic_buffer>&& buffer, rsize_ref &&render_size);

}
}

#endif /* _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_ */

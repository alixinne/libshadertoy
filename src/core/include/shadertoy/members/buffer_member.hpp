#ifndef _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_
#define _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/buffer_output.hpp"
#include "shadertoy/members/basic_member.hpp"

#include "shadertoy/io_resource.hpp"

#include "shadertoy/backends/gx/draw_state.hpp"

namespace shadertoy
{
namespace members
{

/**
 * @brief Output allocator callback
 *
 * This function object returns for a given buffer_output discovered in a
 * program, and a size reference of a swap chain member, the resulting
 * size_reference object and OpenGL internal format of the texture that should
 * be created.
 */
typedef std::function<std::tuple<rsize_ref, GLint>(buffers::buffer_output &, const rsize_ref &)> output_allocator_t;

/**
 * @brief Represents a swap chain member backed by a buffer
 *
 * Here, `buffer` refers to a graphics_buffer, i.e. a buffer which is a member
 * of the graphics pipeline, with texture outputs stored in the associated
 * io_resource.
 */
class shadertoy_EXPORT buffer_member : public basic_member
{
	/// Buffer referenced by this member
	std::shared_ptr<buffers::graphics_buffer> buffer_;

	/// IO resource object that handles texture allocations
	io_resource io_;

	/// OpenGL drawing state
	std::unique_ptr<backends::gx::draw_state> state_;

	/// Rendering size for IO resource
	rsize_ref render_size_;

	/// Default internal format for IO resource
	GLint internal_format_;

	/// Output allocator function
	output_allocator_t output_allocator_;

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
	buffer_member(std::shared_ptr<buffers::graphics_buffer> buffer, rsize_ref render_size,
				  GLint internal_format, member_swap_policy swap_policy);

	virtual ~buffer_member() = default;

	/**
	 * @brief Get the buffer associated with this member
	 *
	 * @return Pointer to the buffer associated with this member
	 */
	inline const std::shared_ptr<buffers::graphics_buffer> &buffer() const { return buffer_; }

	/**
	 * @brief Get the IO resource object that holds this member's textures
	 *
	 * @return Reference to the IO resource object
	 */
	inline const io_resource &io() const { return io_; }

	/**
	 * @brief Get a reference to the OpenGL state
	 *
	 * @return Reference to the OpenGL state
	 */
	inline const backends::gx::draw_state &state() const { return *state_; }

	/**
	 * @brief Get a reference to the OpenGL state
	 *
	 * @return Reference to the OpenGL state
	 */
	inline backends::gx::draw_state &state() { return *state_; }

	/**
	 * @brief Get the output allocator function
	 *
	 * @return Reference to the output allocator function
	 */
	inline const output_allocator_t &output_allocator() const { return output_allocator_; }

	/**
	 * @brief Set the output allocator function
	 *
	 * @param new_allocator New output allocator function
	 */
	inline void output_allocator(output_allocator_t new_allocator)
	{
		output_allocator_ = new_allocator;
	}

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
 * @brief Construct a pointer to a buffer_member
 *
 * @param args Arguments to buffer_member#buffer_member
 *
 * @return Pointer to the constructed buffer_member
 *
 * @see buffer_member#buffer_member
 */
template <typename... Args> std::shared_ptr<buffer_member> make_buffer(Args &&... args)
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
 * @see buffer_member#buffer_member(std::shared_ptr<buffers::graphics_buffer>, rsize_ref &&, GLint, member_swap_policy)
 */
std::shared_ptr<buffer_member> shadertoy_EXPORT make_member(const swap_chain &chain,
															std::shared_ptr<buffers::graphics_buffer> buffer,
															rsize_ref &&render_size);

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
 * @see buffer_member#buffer_member(std::shared_ptr<buffers::graphics_buffer>, rsize_ref &&, GLint, member_swap_policy)
 */
std::shared_ptr<buffer_member>
shadertoy_EXPORT make_member(const swap_chain &chain, std::shared_ptr<buffers::graphics_buffer> buffer,
							 rsize_ref &&render_size, GLint internal_format);

/**
 * @brief Construct a pointer to a buffer_member
 *
 * @param chain           Swap chain this member is being constructed from. The
 *                        non-specified parameters will get their defaults from this chain.
 * @param buffer          Buffer to add to the swap chain
 * @param render_size     Size to render the buffer at when integrated in the swap chain
 * @param swap_policy     Texture swapping policy for this member
 *
 * @return Pointer to the constructed buffer_member
 *
 * @see buffer_member#buffer_member(std::shared_ptr<buffers::graphics_buffer>, rsize_ref &&, GLint, member_swap_policy)
 */
std::shared_ptr<buffer_member>
shadertoy_EXPORT make_member(const swap_chain &chain, std::shared_ptr<buffers::graphics_buffer> buffer,
							 rsize_ref &&render_size, member_swap_policy swap_policy);

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
 * @see buffer_member#buffer_member(std::shared_ptr<buffers::graphics_buffer>, rsize_ref &&, GLint, member_swap_policy)
 */
std::shared_ptr<buffer_member>
shadertoy_EXPORT make_member(const swap_chain &chain, std::shared_ptr<buffers::graphics_buffer> buffer,
							 rsize_ref &&render_size, GLint internal_format, member_swap_policy swap_policy);
} // namespace members
} // namespace shadertoy

#endif /* _SHADERTOY_MEMBERS_BUFFER_MEMBER_HPP_ */

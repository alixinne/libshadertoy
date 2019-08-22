#ifndef _SHADERTOY_INPUTS_BUFFER_INPUT_HPP_
#define _SHADERTOY_INPUTS_BUFFER_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/members/basic_member.hpp"

#include "shadertoy/output_name.hpp"

#include <optional>

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input that uses a swap chain output as its source.
 *
 * Note that setting min_filter to GL_*_MIPMAP_* will trigger automatic
 * generation of mipmaps on every update to the source member of this input,
 * which can impact performance of the resulting chain.
 *
 * This class only holds a \c weak_ptr to the referenced input to prevent
 * cyclic references. This means the target member must be owned by another
 * object for as long as this input object exists.
 */
class shadertoy_EXPORT buffer_input : public basic_input
{
	/// Reference to the member for this input
	std::weak_ptr<members::basic_member> member_;

	/// Output name from the member
	output_name_t output_name_;

	/// Cached output index
	int output_index_;

	/// Find the target output
	std::optional<members::member_output_t> find_output(std::shared_ptr<members::basic_member> member);

	protected:
	/**
	 * @brief Unused, except for determining the target internal format.
	 *
	 * @return The internal format of the loaded texture
	 */
	GLenum load_input() override;

	/// unused
	void reset_input() override;

	/**
	 * @brief Obtain this input's texture object.
	 *
	 * In the case of a member input, it is the source texture
	 * of the associated member, or a null texture if there is
	 * no associated member.
	 *
	 * The member must have been initialized first so its textures
	 * exist.
	 *
	 * @return Pointer to the texture object for this input
	 */
	backends::gx::texture *use_input() override;

	public:
	/**
	 * @brief Initialize a new instance of the buffer_input class with no associated buffer.
	 */
	buffer_input();

	/**
	 * @brief Initialize a new instance of the buffer_input class with \p
	 * member as an associated member.
	 *
	 * @param member      Buffer to use as a source
	 * @param output_name Output index from the target
	 */
	buffer_input(std::weak_ptr<members::basic_member> member, output_name_t output_name = 0);

	/**
	 * @brief Obtain the source member for this input
	 *
	 * @return Pointer to the source member for this input
	 */
	inline const std::weak_ptr<members::basic_member> &member() const { return member_; }

	/**
	 * @brief Set the source member for this input
	 *
	 * As a buffer input is stateless, the reset method does not
	 * need to be called after changing the source member.
	 *
	 * @param new_member New source member
	 */
	inline void member(std::weak_ptr<members::basic_member> new_member) { member_ = new_member; }

	/**
	 * @brief  Obtain the output name for this input
	 *
	 * @return Output index for this input
	 */
	inline output_name_t output_name() const { return output_name_; }

	/**
	 * @brief Set the output name for this input
	 *
	 * @param new_name New output name.
	 */
	inline void output_name(output_name_t new_name) { output_name_ = new_name; }
};
} // namespace inputs
} // namespace shadertoy

#endif /* _SHADERTOY_INPUTS_BUFFER_INPUT_HPP_ */

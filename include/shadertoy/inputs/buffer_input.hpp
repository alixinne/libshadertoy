#ifndef _SHADERTOY_INPUTS_BUFFER_INPUT_HPP_
#define _SHADERTOY_INPUTS_BUFFER_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/basic_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input that uses a buffer as its source.
 */
class shadertoy_EXPORT buffer_input : public basic_input
{
	/// Reference to the buffer for this input
	std::shared_ptr<buffers::basic_buffer> buffer_;

protected:
	/// unused
	void load_input() override;

	/// unused
	void reset_input() override;

	/**
	 * @brief Obtains this input's texture object.
	 *
	 * In the case of a buffer input, it is the source texture
	 * of the associated buffer, or a null texture if there is
	 * no associated buffer.
	 *
	 * The buffer must have been initialized first so its textures
	 * exist.
	 *
	 * @return Pointer to the texture object for this input
	 */
	std::shared_ptr<gl::texture> use_input() override;

public:
	/**
	 * @brief Initializes a new instance of the buffer_input class with no associated buffer.
	 */
	buffer_input();

	/**
	 * @brief Initializes a new instance of the buffer_input class with \p
	 * buffer as an associated buffer.
	 *
	 * @param buffer Buffer to use as a source
	 */
	buffer_input(std::shared_ptr<buffers::basic_buffer> buffer);

	/**
	 * @brief Obtains the source buffer for this input
	 *
	 * @return Pointer to the source buffer for this input
	 */
	inline const std::shared_ptr<buffers::basic_buffer> &buffer() const { return buffer_; }

	/**
	 * @brief Sets the source buffer for this input
	 *
	 * As a buffer input is stateless, the reset method does not
	 * need to be called after changing the source buffer.
	 *
	 * @param new_buffer New source buffer
	 */
	inline void buffer(std::shared_ptr<buffers::basic_buffer> new_buffer) { buffer_ = new_buffer; }
};
}
}

#endif /* _SHADERTOY_INPUTS_BUFFER_INPUT_HPP_ */

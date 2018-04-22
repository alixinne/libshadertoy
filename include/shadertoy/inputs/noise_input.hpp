#ifndef _SHADERTOY_INPUTS_NOISE_INPUT_HPP_
#define _SHADERTOY_INPUTS_NOISE_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/basic_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input that is a texture of random white noise
 */
class shadertoy_EXPORT noise_input : public basic_input
{
	/// Texture object
	std::shared_ptr<gl::texture> texture_;

	/// Size of the generated texture
	rsize_ref size_;

protected:
	/**
	 * @brief Loads this input's contents
	 */
	void load_input() override;

	/**
	 * @brief Resets this input's contents
	 */
	void reset_input() override;

	/**
	 * @brief Obtains this input's texture object
	 */
	std::shared_ptr<gl::texture> use_input() override;

public:
	/**
	 * @brief Initializes a new instance of the noise_input class of the given size
	 *
	 * @param size Size object to define the noise texture size
	 */
	noise_input(rsize_ref &&size);
};
}
}

#endif /* _SHADERTOY_INPUTS_NOISE_INPUT_HPP_ */

#ifndef _SHADERTOY_INPUTS_ERROR_INPUT_HPP_
#define _SHADERTOY_INPUTS_ERROR_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/basic_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input that can be used as an error detector, i.e. when
 *        some other texture cannot be loaded.
 */
class shadertoy_EXPORT error_input : public basic_input
{
	/// Texture object
	std::shared_ptr<gl::texture> texture_;

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
	 * @brief Initializes a new instance of the error_input class
	 */
	error_input();
};
}
}

#endif /* _SHADERTOY_INPUTS_ERROR_INPUT_HPP_ */

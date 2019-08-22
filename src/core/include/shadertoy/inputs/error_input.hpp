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
	std::unique_ptr<backends::gx::texture> texture_;

protected:
	/**
	 * @brief Load this input's contents
	 *
	 * @return The internal format of the loaded texture
	 */
	GLenum load_input() override;

	/**
	 * @brief Reset this input's contents
	 */
	void reset_input() override;

	/**
	 * @brief Obtain this input's texture object
	 *
	 * @return OpenGL texture object representing the error texture
	 */
	backends::gx::texture *use_input() override;

public:
	/**
	 * @brief Initialize a new instance of the error_input class
	 */
	error_input();
};
}
}

#endif /* _SHADERTOY_INPUTS_ERROR_INPUT_HPP_ */

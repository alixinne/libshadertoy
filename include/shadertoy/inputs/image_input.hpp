#ifndef _SHADERTOY_INPUTS_IMAGE_INPUT_HPP_
#define _SHADERTOY_INPUTS_IMAGE_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/basic_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input based on an image, potentially loaded
 * from the disk or another source.
 */
class shadertoy_EXPORT image_input : public basic_input
{
	/// Texture object to hold the image data
	std::unique_ptr<gl::texture> image_texture_;

protected:
	/**
	 * @brief Implemented by derived classes to provide the image decoding logic
	 *
	 * @param[out] format Internal format of the resulting image
	 *
	 * @return OpenGL texture representing the image
	 */
	virtual std::unique_ptr<gl::texture> load_image(GLenum &format) = 0;

	/**
	 * @brief Load the input's contents.
	 *
	 * Classes implementing image_input should implement load_image to
	 * support this behavior.
	 *
	 * @return The internal format of the loaded texture
	 */
	GLenum load_input() final;

	/**
	 * @brief Reset the input's contents.
	 */
	void reset_input() final;

	/**
	 * @brief Use this input in a rendering pass.
	 *
	 * @return OpenGL texture object representing the input
	 */
	gl::texture *use_input() final;

	/**
	 * @brief Initialize a new instance of the image_input class.
	 */
	image_input();
};
}
}

#endif /* _SHADERTOY_INPUTS_IMAGE_INPUT_HPP_ */

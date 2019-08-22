#ifndef _SHADERTOY_INPUTS_TEXTURE_INPUT_HPP_
#define _SHADERTOY_INPUTS_TEXTURE_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/basic_input.hpp"

#include "shadertoy/backends/gx/texture.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input based on an texture, potentially loaded
 * from the disk or another source.
 */
class shadertoy_EXPORT texture_input : public basic_input
{
	/// Texture object to hold the image data
	std::unique_ptr<backends::gx::texture> image_texture_;

	/// Internal format of the texture
	GLenum internal_format_;

protected:
	/**
	 * @brief Load the input's contents.
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
	backends::gx::texture *use_input() final;

public:
	/**
	 * @brief Initialize a new instance of the texture_input class.
	 * 
	 * @param image_texture   Texture to return for this input
	 * @param internal_format Internal format for the texture
	 */
	texture_input(std::unique_ptr<backends::gx::texture> image_texture, GLenum internal_format);

	/**
	 * @brief Get a reference to the underlying texture object
	 */
	inline const backends::gx::texture &texture() const
	{ return *image_texture_; }
};
}
}

#endif /* _SHADERTOY_INPUTS_TEXTURE_INPUT_HPP_ */

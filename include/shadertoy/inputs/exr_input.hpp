#ifndef _SHADERTOY_INPUTS_EXR_INPUT_HPP_
#define _SHADERTOY_INPUTS_EXR_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/file_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input that is loaded from a file
 *        using the OpenEXR library.
 */
class shadertoy_EXPORT exr_input : public file_input
{
protected:
	/**
	 * @brief Load the image from filename
	 *
	 * @param      filename Filename to load the image from
	 * @param      vflip    true if the image should be flipped vertically while loading
	 * @param[out] format   Internal format of the resulting image
	 *
	 * @return OpenGL texture representing the image
	 */
	std::unique_ptr<gl::texture> load_file(const std::string &filename, bool vflip, GLenum &format) override;

public:
	/**
	 * @brief Initialize a new instance of the exr_input class
	 *
	 * This instance will have no filename setup, therefore it will not load
	 * any texture.
	 */
	exr_input();

	/**
	 * @brief Initialize a new instance of the exr_input class
	 * with a default filename
	 *
	 * @param filename Filename to load the image from
	 */
	explicit exr_input(const std::string &filename);

	/**
	 * @brief Get a value indicating if this input type is supported
	 *
	 * @return true if it is supported, false otherwise
	 */
	static bool supported();
};
}
}

#endif /* _SHADERTOY_INPUTS_EXR_INPUT_HPP_ */

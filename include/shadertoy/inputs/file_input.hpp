#ifndef _SHADERTOY_INPUTS_FILE_INPUT_HPP_
#define _SHADERTOY_INPUTS_FILE_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/image_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an image input that is loaded from a file.
 */
class shadertoy_EXPORT file_input : public image_input
{
	/// Filename to load as an input
	std::string filename_;

	/// true if the image should be flipped vertically
	bool vflip_;

protected:
	/**
	 * @brief Load the image from filename
	 *
	 * @param filename Filename to load the image from
	 * @param vflip    true if the image should be flipped vertically while loading
	 *
	 * @return OpenGL texture representing the image
	 */
	virtual std::unique_ptr<gl::texture> load_file(const std::string &filename, bool vflip) = 0;

	/**
	 * @brief Load the decoded image into device memory.
	 *
	 * Derived classes should implement load_file.
	 *
	 * @return OpenGL texture representing the image
	 */
	std::unique_ptr<gl::texture> load_image() final;

	/**
	 * @brief Initialize a new instance of the file_input class.
	 */
	file_input();

	/**
	 * @brief Initialie a new instance of the file_input class with a default
	 * filename
	 *
	 * @param filename Default filename to load the image from
	 */
	explicit file_input(std::string filename);

	public:
	/**
	 * @brief Obtain the filename this input will be loaded from.
	 *
	 * @return Filename this input will be loaded from
	 */
	inline const std::string &filename() const { return filename_; }

	/**
	 * @brief Set the filename this input will be loaded from.
	 *
	 * Note that this method does not invalidate the input contents,
	 * so reset should be called to trigger a reload step.
	 *
	 * @param new_filename New filename to load the image from
	 */
	void filename(const std::string &new_filename) { filename_ = new_filename; }

	/**
	 * @brief Obtain the vflip flag status
	 *
	 * @return true if the image will be flipped on loading
	 */
	inline bool vflip() const { return vflip_; }

	/**
	 * @brief Set the vflip flag
	 *
	 * Note that this method does not invalidate the input contents,
	 * so reset should be called to trigger a reload step.
	 *
	 * @param new_vflip New value of the vflip tag
	 */
	void vflip(bool new_vflip) { vflip_ = new_vflip; }
};
}
}

#endif /* _SHADERTOY_INPUTS_FILE_INPUT_HPP_ */

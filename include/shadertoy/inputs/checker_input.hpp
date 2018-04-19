#ifndef _SHADERTOY_INPUTS_CHECKER_INPUT_HPP_
#define _SHADERTOY_INPUTS_CHECKER_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/basic_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input that is a checkerboard texture
 */
class shadertoy_EXPORT checker_input : public basic_input
{
	/// Texture object
	std::shared_ptr<gl::texture> texture_;

	/// Size of the generated texture
	rsize_ref size_;

	/// Size of the tiles
	size_t tile_size_;

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
	 * @brief Initializes a new instance of the checker_input class of the given size
	 *
	 * @param size Size object to define the checker texture size
	 */
	checker_input(const rsize_ref &size);

	/**
	 * @brief Initializes a new instance of the checker_input class of the given size
	 *
	 * @param size      Size object to define the checker texture size
	 * @param tile_size Size of the checkerboard tiles
	 */
	checker_input(const rsize_ref &size, size_t tile_size);

	/**
	 * @brief Obtains the current checkerboard tile size
	 *
	 * @return Tile size
	 */
	inline size_t tile_size() const { return tile_size_; }

	/**
	 * @brief Sets the current checkerboard tile size
	 *
	 * The reset() method must be called manually after changing this parameter.
	 *
	 * @param new_tile_size New tile size
	 */
	void tile_size(size_t new_tile_size) { tile_size_ = new_tile_size; }
};
}
}

#endif /* _SHADERTOY_INPUTS_CHECKER_INPUT_HPP_ */

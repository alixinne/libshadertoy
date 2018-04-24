#ifndef _SHADERTOY_INPUTS_BASIC_INPUT_HPP_
#define _SHADERTOY_INPUTS_BASIC_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include <memory>

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents a texture input to a buffer
 */
class shadertoy_EXPORT basic_input
{
	/// Sampler object to configure the texture unit for this input
	gl::sampler sampler_;

	/// true if this input has been loaded
	bool loaded_;

protected:
	/**
	 * @brief Loads this input's contents.
	 *
	 * This method must be implemented by derived classes as part
	 * of their loading routine.
	 */
	virtual void load_input() = 0;

	/**
	 * @brief Resets this input's contents.
	 *
	 * This method must be implemented by derived classes as part
	 * of their reset routine.
	 */
	virtual void reset_input() = 0;

	/**
	 * @brief Obtains this input's texture object.
	 *
	 * This method must be implemented by derived classes in order
	 * to return a texture object to be used on the device. Note
	 * that an empty pointer can be returned, and will be replaced
	 * by an error texture for debugging.
	 *
	 * @return Pointer to the texture object for this input
	 */
	virtual std::shared_ptr<gl::texture> use_input() = 0;

	/**
	 * @brief Initializes a new instance of the basic_input class.
	 */
	basic_input();

public:
	/**
	 * @brief Loads this input's contents.
	 *
	 * If the input has already been loaded, it will not be loaded again. In
	 * order to load it again, use the reset() method.
	 */
	void load();

	/**
	 * @brief Resets this input's contents.
	 *
	 * If the input has not been loaded, this method is a no-op.
	 */
	void reset();

	/**
	 * @brief Uses this input for a rendering pass.
	 *
	 * If the input has not yet been loaded, it will be loaded.
	 *
	 * @return Pointer to the texture object for this input.
	 */
	std::shared_ptr<gl::texture> use();

	/**
	 * @brief Obtains the sampler object for this input
	 *
	 * @return Reference to the sampler object
	 */
	inline gl::sampler &sampler() { return sampler_; }

	/**
	 * @brief Gets the minification filter of this input's sampler
	 *
	 * @return Current value of the GL_MIN_FILTER parameter
	 */
	GLint min_filter();

	/**
	 * @brief Sets the minification filter of this input's sampler
	 *
	 * @param new_min_filter New minification filter
	 */
	void min_filter(GLint new_min_filter);

	/**
	 * @brief Sets the magnification filter of this input's sampler
	 *
	 * @param new_mag_filter New magnification filter
	 */
	void mag_filter(GLint new_mag_filter);

	/**
	 * @brief Sets the wrap parameter of this input's sampler
	 *
	 * @param new_wrap New wrap parameter
	 */
	void wrap(GLint new_wrap);

	/**
	 * @brief Binds the sampler and its texture to the given unit
	 *
	 * @param unit Unit to bind to
	 *
	 * @return The bound texture
	 */
	std::shared_ptr<gl::texture> bind(GLuint unit);
};
}
}

#endif /* _SHADERTOY_INPUTS_BASIC_INPUT_HPP_ */
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
 *
 * The default minification (resp. magnification) filter parameters default to GL_NEAREST.
 */
class shadertoy_EXPORT basic_input
{
	/// Sampler object to configure the texture unit for this input
	gl::sampler sampler_;

	/// true if this input has been loaded
	bool loaded_;

protected:
	/**
	 * @brief Load this input's contents.
	 *
	 * This method must be implemented by derived classes as part
	 * of their loading routine.
	 */
	virtual void load_input() = 0;

	/**
	 * @brief Reset this input's contents.
	 *
	 * This method must be implemented by derived classes as part
	 * of their reset routine.
	 */
	virtual void reset_input() = 0;

	/**
	 * @brief Obtain this input's texture object.
	 *
	 * This method must be implemented by derived classes in order
	 * to return a texture object to be used on the device. Note
	 * that an empty pointer can be returned, and will be replaced
	 * by an error texture for debugging.
	 *
	 * @return Pointer to the texture object for this input. The returned
	 * pointer must have the same lifetime semantics as required by
	 * basic_input#use.
	 */
	virtual gl::texture *use_input() = 0;

	/**
	 * @brief Initialize a new instance of the basic_input class.
	 */
	basic_input();

public:
	virtual ~basic_input() = default;

	/**
	 * @brief Load this input's contents.
	 *
	 * If the input has already been loaded, it will not be loaded again. In
	 * order to load it again, use the reset() method.
	 */
	void load();

	/**
	 * @brief Reset this input's contents.
	 *
	 * If the input has not been loaded, this method is a no-op.
	 */
	void reset();

	/**
	 * @brief Use this input for a rendering pass.
	 *
	 * If the input has not yet been loaded, it will be loaded.
	 *
	 * @return Pointer to the texture object for this input. The pointer is
	 * guaranteed to be valid as long as the basic_input instance exists and is
	 * not altered by any state-changing such as basic_input#reset or
	 * equivalents in derived classes.
	 */
	gl::texture *use();

	/**
	 * @brief Obtain the sampler object for this input
	 *
	 * @return Reference to the sampler object
	 */
	inline gl::sampler &sampler() { return sampler_; }

	/**
	 * @brief Get the minification filter of this input's sampler
	 *
	 * @return Current value of the GL_MIN_FILTER parameter
	 */
	GLint min_filter() const;

	/**
	 * @brief Set the minification filter of this input's sampler
	 *
	 * @param new_min_filter New minification filter
	 */
	void min_filter(GLint new_min_filter);

	/**
	 * @brief Get the magnification filter of this input's sampler
	 *
	 * @return Current value of the GL_MAG_FILTER parameter
	 */
	GLint mag_filter() const;

	/**
	 * @brief Set the magnification filter of this input's sampler
	 *
	 * @param new_mag_filter New magnification filter
	 */
	void mag_filter(GLint new_mag_filter);

	/**
	 * @brief Set the wrap parameter of this input's sampler
	 *
	 * @param new_wrap New wrap parameter
	 */
	void wrap(GLint new_wrap);

	/**
	 * @brief Bind the sampler and its texture to the given unit
	 *
	 * @param unit Unit to bind to
	 *
	 * @return The bound texture. See basic_input#use for details.
	 */
	gl::texture *bind(GLuint unit);
};
}
}

#endif /* _SHADERTOY_INPUTS_BASIC_INPUT_HPP_ */

#ifndef _SHADERTOY_PROGRAM_INPUT_HPP_
#define _SHADERTOY_PROGRAM_INPUT_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief Represents an input texture to shader code
 */
class shadertoy_EXPORT program_input
{
	/// Name of the sampler type in GLSL
	std::string sampler_type_;

	/// Name of the sampler in GLSL
	std::string sampler_name_;

	/// Input object
	std::shared_ptr<inputs::basic_input> input_;

public:
	/**
	 * @brief Create a new unnamed input
	 *
	 * Unnamed inputs are allowed for compatibility with iChannel*,
	 * which forces their type to be sampler2D
	 */
	program_input();

	/**
	 * @brief Create a new unnamed input
	 *
	 * Unnamed inputs are allowed for compatibility with iChannel*,
	 * which forces their type to be sampler2D
	 * 
	 * @param input Initial input value
	 */
	program_input(std::shared_ptr<inputs::basic_input> input);

	/**
	 * @brief Create a new empty input
	 *
	 * The input type defaults to sampler2D.
	 *
	 * @param sampler_name GLSL name of the sampler
	 */
	program_input(const std::string &sampler_name);

	/**
	 * @brief Create a new input with a given value
	 *
	 * The input type defaults to sampler2D.
	 *
	 * @param sampler_name GLSL name of the sampler
	 * @param input        Initial value of the input
	 */
	program_input(const std::string &sampler_name, std::shared_ptr<inputs::basic_input> input);

	/**
	 * @brief Create a new empty input
	 *
	 * @param sampler_type GLSL type of the sampler
	 * @param sampler_name GLSL name of the sampler
	 */
	program_input(const std::string &sampler_type, const std::string &sampler_name);

	/**
	 * @brief Create a new input with a given value
	 *
	 * @param sampler_type GLSL type of the sampler
	 * @param sampler_name GLSL name of the sampler
	 * @param input        Initial input value
	 */
	program_input(const std::string &sampler_type, const std::string &sampler_name,
			std::shared_ptr<inputs::basic_input> input);

	/**
	 * @brief Get the sampler type name
	 *
	 * @return Name of the GLSL sampler type
	 */
	inline const std::string sampler_type() const
	{ return sampler_type_; }

	/**
	 * @brief Get the sampler name
	 *
	 * @return Name of this sampler
	 */
	inline const std::string sampler_name() const
	{ return sampler_name_; }

	/**
	 * @brief Get the input object
	 *
	 * @return Reference to the input object
	 */
	inline const std::shared_ptr<inputs::basic_input> &input() const
	{ return input_; }

	/**
	 * @brief Get the input object
	 *
	 * @return Reference to the input object
	 */
	inline std::shared_ptr<inputs::basic_input> &input()
	{ return input_; }

	/**
	 * @brief Set the input object
	 *
	 * @param new_input New input object
	 */
	inline void input(std::shared_ptr<inputs::basic_input> new_input)
	{ input_ = new_input; }

	/**
	 * @brief Get the GLSL definition for this sampler
	 *
	 * @return GLSL source for defining this sampler in a shader
	 */
	std::string definition_string() const;
};

/**
 * @brief Build a program_input according to the arguments to its constructor
 *
 * @param args Arguments to program_input#program_input
 * @see program_input#program_input
 *
 * @return Pointer to the constructed program_input
 */
template<typename... Args>
std::shared_ptr<program_input> make_input(Args&&... args)
{
	return std::make_shared<program_input>(std::forward<Args>(args)...);
}
}

#endif /* _SHADERTOY_PROGRAM_INPUT_HPP_ */

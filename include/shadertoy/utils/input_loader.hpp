#ifndef _SHADERTOY_UTILS_INPUT_LOADER_HPP_
#define _SHADERTOY_UTILS_INPUT_LOADER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/utils/input_factory.hpp"

#include <memory>
#include <set>

namespace shadertoy
{
namespace utils
{

/**
 * @brief Represents an input loader that determines which input type to create based on its specification
 * and a set of input factories
 */
class shadertoy_EXPORT input_loader
{
	std::set<std::unique_ptr<input_factory>, input_factory_ptr_comparator> factories_;

public:
	/**
	 * @brief Create an input loader that supports builtin types
	 */
	input_loader();

	/**
	 * @brief Add an input factory
	 *
	 * @param factory New input factory
	 */
	void add(std::unique_ptr<input_factory> &&factory);

	/**
	 * @brief Create an input based on its specification
	 *
	 * The specification is a URI which indicates how to create the input as
	 * well as supplementary parameters to create the input object. Currently,
	 * the following schemes are supported:
	 *
	 *  * file (path to an image file)
	 *  * noise (no parameters)
	 *
	 * The file URI scheme is dispatched to the corresponding loader depending
	 * on the file name extension, and supported loaders that have been built
	 * into the library.
	 *
	 * The noise URI scheme creates an inputs::noise_input with the given
	 * parameters.
	 *
	 * @param input URI that represents the input to be created
	 *
	 * @return Created input, or null of no input factory was found
	 */
	std::shared_ptr<inputs::basic_input> create(const std::string &input) const;
};
}
}

#endif /* _SHADERTOY_UTILS_INPUT_LOADER_HPP_ */

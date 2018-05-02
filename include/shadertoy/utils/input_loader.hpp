#ifndef _SHADERTOY_UTILS_INPUT_LOAD_HPP_
#define _SHADERTOY_UTILS_INPUT_LOAD_HPP_

#include "shadertoy/pre.hpp"

#include <memory>
#include <set>

namespace shadertoy
{
namespace utils
{

/**
 * @brief Input factory for the input_loader
 */
class shadertoy_EXPORT input_factory
{
public:
	/**
	 * @brief Priority of this factory. Higher priorities will be used first.
	 *
	 * @return Integer value indicating the priority of this input_factory compared to others
	 */
	virtual int priority() const = 0;

	/**
	 * @brief Check if the specification is supported by this input factory
	 *
	 * @param spec Specification of the input to create, extracted from the input URI
	 *
	 * @return true if this specification is supported, false otherwise
	 */
	virtual bool supported(const std::string &spec) const = 0;

	/**
	 * @brief Actually creates the input based on its specification
	 *
	 * @param spec Specification of the input to create, extracted from the input URI
	 *
	 * @return Pointer to the constructed input
	 */
	virtual std::shared_ptr<inputs::basic_input> create(const std::string &spec) const = 0;

	/**
	 * @brief Get the type string for this factory
	 *
	 * The type string is used as a URI scheme for the input specification. For
	 * file based inputs, this can be "file". An HTTP(S) factory could be
	 * implemented using this.
	 *
	 * @return Type string for this factory
	 */
	virtual const std::string &type_name() const = 0;

	/**
	 * @brief Compare this input_factory with \p rhs
	 *
	 * @param rhs Right-hand side of the comparison
	 *
	 * @return true if *this &lt; \p rhs
	 */
	bool operator<(const input_factory &rhs);
};

/// Comparator object for std::unique_ptr&lt;input_factory&gt;
struct input_factory_ptr_comparator
{
	/**
	 * @brief Compare \p lhs and \p rhs according to input_factor::operator&lt; on the pointed values
	 *
	 * @param lhs Left hand side of comparison
	 * @param rhs Right hand side of comparison
	 *
	 * @return true if lhs &lt; rhs
	 */
	bool operator()(const std::unique_ptr<input_factory> &lhs, const std::unique_ptr<input_factory> &rhs) const;
};

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

#endif /* _SHADERTOY_UTILS_INPUT_LOAD_HPP_ */

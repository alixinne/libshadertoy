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
	/// Priority of this factory. Higher priorities will be used first
	virtual int priority() const = 0;

	/// Returns true if the specification is supported
	virtual bool supported(const std::string &spec) const = 0;

	/// Actually creates the input based on its specification
	virtual std::shared_ptr<inputs::basic_input> create(const std::string &spec) const = 0;

	/// Returns the type string for this factory
	virtual const std::string &type_name() const = 0;

	bool operator<(const input_factory &rhs);
};

struct input_factory_ptr_comparator
{
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
	 * @brief Creates an input loader that supports builtin types
	 */
	input_loader();

	/**
	 * @brief Adds an input factory
	 *
	 * @param factory New input factory
	 */
	void add(std::unique_ptr<input_factory> &&factory);

	/**
	 * @brief Creates an input based on its specification
	 *
	 * @return Created input, or null of no input factory was found
	 */
	std::shared_ptr<inputs::basic_input> create(const std::string &input) const;
};
}
}

#endif /* _SHADERTOY_UTILS_INPUT_LOAD_HPP_ */

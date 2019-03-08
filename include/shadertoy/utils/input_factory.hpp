#ifndef _SHADERTOY_UTILS_INPUT_FACTORY_HPP_
#define _SHADERTOY_UTILS_INPUT_FACTORY_HPP_

#include "shadertoy/pre.hpp"

#include <map>
#include <memory>
#include <string>

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
	virtual ~input_factory() = default;

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
	virtual bool supported(const std::map<std::string, std::string> &spec) const = 0;

	/**
	 * @brief Actually creates the input based on its specification
	 *
	 * @param spec Specification of the input to create, extracted from the input URI
	 *
	 * @return Pointer to the constructed input
	 */
	virtual std::unique_ptr<inputs::basic_input> create(const std::map<std::string, std::string> &spec) const = 0;

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
}
}

#endif /* _SHADERTOY_UTILS_INPUT_FACTORY_HPP_ */

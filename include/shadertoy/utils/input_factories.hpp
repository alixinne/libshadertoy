#ifndef _SHADERTOY_UTILS_INPUT_FACTORIES_HPP_
#define _SHADERTOY_UTILS_INPUT_FACTORIES_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/utils/input_factory.hpp"

namespace shadertoy
{
namespace utils
{

/** @cond INTERNAL */

class soil_input_factory : public input_factory
{
	const std::string type_name_;

public:
	inline int priority() const override { return 10; }

	bool supported(const std::map<std::string, std::string> &spec) const override;

	std::unique_ptr<inputs::basic_input> create(const std::map<std::string, std::string> &spec) const override;

	inline const std::string &type_name() const override { return type_name_; }

	soil_input_factory();
};

class jpeg_input_factory : public input_factory
{
	const std::string type_name_;

public:
	inline int priority() const override { return 45; }

	bool supported(const std::map<std::string, std::string> &spec) const override;

	std::unique_ptr<inputs::basic_input> create(const std::map<std::string, std::string> &spec) const override;

	inline const std::string &type_name() const override { return type_name_; }

	jpeg_input_factory();
};

class exr_input_factory : public input_factory
{
	const std::string type_name_;

public:
	inline int priority() const override { return 50; }

	bool supported(const std::map<std::string, std::string> &spec) const override;

	std::unique_ptr<inputs::basic_input> create(const std::map<std::string, std::string> &spec) const override;

	inline const std::string &type_name() const override { return type_name_; }

	exr_input_factory();
};

class noise_input_factory : public input_factory
{
	const std::string type_name_;

public:
	inline int priority() const override { return 50; }

	inline bool supported(const std::map<std::string, std::string> &spec) const override { return true; }

	std::unique_ptr<inputs::basic_input> create(const std::map<std::string, std::string> &spec) const override;

	inline const std::string &type_name() const override { return type_name_; }

	noise_input_factory();
};

class checker_input_factory : public input_factory
{
	const std::string type_name_;

public:
	inline int priority() const override { return 50; }

	inline bool supported(const std::map<std::string, std::string> &spec) const override { return true; }

	std::unique_ptr<inputs::basic_input> create(const std::map<std::string, std::string> &spec) const override;

	inline const std::string &type_name() const override { return type_name_; }

	checker_input_factory();
};

/** @endcond */
}
}

#endif /* _SHADERTOY_UTILS_INPUT_FACTORIES_HPP_ */

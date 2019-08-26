#include <sstream>
#include <utility>

#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/program_input.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;

using shadertoy::utils::error_assert;

program_input_type program_input::type_from_sampler(const std::string &sampler_type)
{
#if SHADERTOY_HAS_IMAGE_LOAD_STORE
	if (sampler_type.find("image") != std::string::npos)
		return program_input_type::image;
#endif
	if (sampler_type.find("sampler") != std::string::npos)
		return program_input_type::sampler;
	return program_input_type::unknown;
}

program_input::program_input() : sampler_type_("sampler2D"), type_(type_from_sampler(sampler_type_))
{
}

program_input::program_input(const std::shared_ptr<inputs::basic_input> &input)
: sampler_type_("sampler2D"),

  input_(input), type_(type_from_sampler(sampler_type_))
{
	error_assert(input != nullptr, "Input for program_input cannot be empty");
}

program_input::program_input(const std::string &sampler_name)
: sampler_type_("sampler2D"), sampler_name_(sampler_name), type_(type_from_sampler(sampler_type_))

{
	error_assert(!sampler_name.empty(), "Sampler name for program_input cannot be empty");
}

program_input::program_input(const std::string &sampler_name, const std::shared_ptr<inputs::basic_input> &input)
: sampler_type_("sampler2D"), sampler_name_(sampler_name), input_(input),
  type_(type_from_sampler(sampler_type_))
{
	error_assert(!sampler_name.empty(), "Sampler name for program_input cannot be empty");
	error_assert(input != nullptr, "Input for program_input cannot be empty");
}

program_input::program_input(std::string sampler_type, std::string sampler_name)
: sampler_type_(std::move(sampler_type)), sampler_name_(std::move(sampler_name)),
  type_(type_from_sampler(sampler_type_))

{
	error_assert(!sampler_type_.empty(), "Sampler type for program_input cannot be empty");
	error_assert(!sampler_name_.empty(), "Sampler name for program_input cannot be empty");
}

program_input::program_input(std::string memory_qualifier, std::string sampler_type, std::string sampler_name)
: sampler_type_(std::move(sampler_type)), sampler_name_(std::move(sampler_name)),
  memory_qualifier_(memory_qualifier), type_(type_from_sampler(sampler_type_))

{
	error_assert(!memory_qualifier_.empty(), "Memory qualifier for program_input cannot be empty");
	error_assert(!sampler_type_.empty(), "Sampler type for program_input cannot be empty");
	error_assert(!sampler_name_.empty(), "Sampler name for program_input cannot be empty");
}

program_input::program_input(std::string sampler_type, std::string sampler_name,
							 const std::shared_ptr<inputs::basic_input> &input)
: sampler_type_(std::move(sampler_type)), sampler_name_(std::move(sampler_name)), input_(input),
  type_(type_from_sampler(sampler_type_))
{
	error_assert(!sampler_type_.empty(), "Sampler type for program_input cannot be empty");
	error_assert(!sampler_name_.empty(), "Sampler name for program_input cannot be empty");
	error_assert(input != nullptr, "Input for program_input cannot be empty");
}

program_input::program_input(std::string memory_qualifier, std::string sampler_type,
							 std::string sampler_name, const std::shared_ptr<inputs::basic_input> &input)
: sampler_type_(std::move(sampler_type)), sampler_name_(std::move(sampler_name)),
  memory_qualifier_(memory_qualifier), input_(input), type_(type_from_sampler(sampler_type_))
{
	error_assert(!memory_qualifier_.empty(), "Memory qualifier for program_input cannot be empty");
	error_assert(!sampler_type_.empty(), "Sampler type for program_input cannot be empty");
	error_assert(!sampler_name_.empty(), "Sampler name for program_input cannot be empty");
	error_assert(input != nullptr, "Input for program_input cannot be empty");
}

std::string program_input::definition_string() const
{
	std::stringstream ss;
	if (sampler_name_.empty())
	{
		ss << "/* unnamed " << sampler_type_ << " input */";
	}
	else
	{
		if (!memory_qualifier_.empty())
			ss << memory_qualifier_ << " ";
		ss << "uniform " << sampler_type_ << " " << sampler_name_ << ";";
	}
	return ss.str();
}

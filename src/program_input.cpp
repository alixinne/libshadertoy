#include <sstream>
#include <utility>

#include <epoxy/gl.h>

#include "shadertoy/program_input.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;

using shadertoy::utils::error_assert;

program_input::program_input() : sampler_type_("sampler2D") {}

program_input::program_input(const std::shared_ptr<inputs::basic_input> &input)
: sampler_type_("sampler2D"),

  input_(input)
{
	error_assert(input != nullptr, "Input for program_input cannot be empty");
}

program_input::program_input(const std::string &sampler_name)
: sampler_type_("sampler2D"), sampler_name_(sampler_name)

{
	error_assert(!sampler_name.empty(), "Sampler name for program_input cannot be empty");
}

program_input::program_input(const std::string &sampler_name, const std::shared_ptr<inputs::basic_input> &input)
: sampler_type_("sampler2D"), sampler_name_(sampler_name), input_(input)
{
	error_assert(!sampler_name.empty(), "Sampler name for program_input cannot be empty");
	error_assert(input != nullptr, "Input for program_input cannot be empty");
}

program_input::program_input(std::string sampler_type, std::string sampler_name)
: sampler_type_(std::move(sampler_type)), sampler_name_(std::move(sampler_name))

{
	error_assert(!sampler_type_.empty(), "Sampler type for program_input cannot be empty");
	error_assert(!sampler_name_.empty(), "Sampler name for program_input cannot be empty");
}

program_input::program_input(std::string sampler_type, std::string sampler_name,
							 const std::shared_ptr<inputs::basic_input> &input)
: sampler_type_(std::move(sampler_type)), sampler_name_(std::move(sampler_name)), input_(input)
{
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
		ss << "uniform " << sampler_type_ << " " << sampler_name_ << ";";
	}
	return ss.str();
}

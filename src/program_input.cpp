#include <cassert>
#include <sstream>

#include <epoxy/gl.h>

#include "shadertoy/program_input.hpp"

using namespace shadertoy;

program_input::program_input()
	: sampler_type_("sampler2D"),
	sampler_name_(),
	input_()
{}

program_input::program_input(std::shared_ptr<inputs::basic_input> input)
	: sampler_type_("sampler2D"),
	sampler_name_(),
	input_(input)
{
	assert(input);
}

program_input::program_input(const std::string &sampler_name)
	: sampler_type_("sampler2D"),
	sampler_name_(sampler_name),
	input_()
{
	assert(!sampler_name.empty());
}

program_input::program_input(const std::string &sampler_name, std::shared_ptr<inputs::basic_input> input)
	: sampler_type_("sampler2D"),
	sampler_name_(sampler_name),
	input_(input)
{
	assert(!sampler_name.empty());
	assert(input);
}

program_input::program_input(const std::string &sampler_type, const std::string &sampler_name)
	: sampler_type_(sampler_type),
	sampler_name_(sampler_name),
	input_()
{
	assert(!sampler_type_.empty());
	assert(!sampler_name_.empty());
}

program_input::program_input(const std::string &sampler_type, const std::string &sampler_name,
		std::shared_ptr<inputs::basic_input> input)
	: sampler_type_(sampler_type),
	sampler_name_(sampler_name),
	input_(input)
{
	assert(!sampler_type_.empty());
	assert(!sampler_name_.empty());
	assert(input);
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

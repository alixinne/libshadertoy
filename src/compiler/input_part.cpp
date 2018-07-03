#include <sstream>

#include "shadertoy/compiler/input_part.hpp"

using namespace shadertoy;
using namespace shadertoy::compiler;

input_part::input_part(const std::string &name, const std::deque<program_input> &inputs)
	: cloneable_part(name),
	inputs_(inputs)
{
}

input_part::operator bool() const
{
	return true;
}

std::vector<std::pair<std::string, std::string>> input_part::sources() const
{
	std::stringstream ss;

	for (auto &input : inputs_)
		ss << input.definition_string() << std::endl;

	return { std::make_pair(name(), ss.str()) };
}

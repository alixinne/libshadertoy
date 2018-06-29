#include <cassert>

#include "shadertoy/compiler/basic_part.hpp"

using namespace shadertoy::compiler;

basic_part::basic_part(const std::string &name)
	: name_(name)
{
	assert(!name.empty());
}

basic_part::~basic_part()
{
}

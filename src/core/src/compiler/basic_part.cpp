#include "shadertoy/compiler/basic_part.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy::compiler;

using shadertoy::utils::error_assert;

basic_part::basic_part(const std::string &name) : name_(name)
{
	error_assert(!name.empty(), "The name of a template part cannot be empty");
}

basic_part::~basic_part() = default;

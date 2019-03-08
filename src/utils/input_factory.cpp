#include "shadertoy/utils/input_factory.hpp"

using namespace shadertoy::utils;

bool input_factory::operator<(const input_factory &rhs)
{
	if (this == &rhs)
	{
		return false;
	}
	if (rhs.priority() < priority())
	{
		return true;
	}
	if (rhs.priority() > priority())
	{
		return false;
	}
	return type_name() < rhs.type_name();
}

bool input_factory_ptr_comparator::operator()(const std::unique_ptr<input_factory> &lhs, const std::unique_ptr<input_factory> &rhs) const
{
	return *lhs < *rhs;
}

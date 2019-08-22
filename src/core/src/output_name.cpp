#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/output_name.hpp"

using shadertoy::output_name_info_t;
using shadertoy::output_name_t;

bool shadertoy::operator==(const output_name_info_t &name_info, const output_name_t &name)
{
	return std::visit(
	[&name_info](const auto &name) -> bool {
		if constexpr (std::is_same_v<const std::string &, decltype(name)>)
		{
			return name == std::get<0>(name_info);
		}
		else
		{
			return name == std::get<1>(name_info);
		}
	},
	name);
}

bool shadertoy::operator!=(const output_name_info_t &name_info, const output_name_t &name)
{
	return !(name_info == name);
}

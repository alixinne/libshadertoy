#include <algorithm>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/sources/stack_source.hpp"

using namespace shadertoy::backends;
using namespace shadertoy::sources;

stack_source::stack_source(std::vector<std::unique_ptr<basic_source>> sources) : sources_(std::move(sources))
{
}

std::vector<std::pair<std::string, std::string>> stack_source::get_source(GLenum shader_type) const
{
	for (auto it = sources_.rbegin(); it != sources_.rend(); ++it)
	{
		if ((*it)->has_source(shader_type))
			return (*it)->get_source(shader_type);
	}

	throw shadertoy::shadertoy_error("unsupported shader type for this source");
}

std::shared_ptr<gx::shader> stack_source::get_precompiled(GLenum shader_type) const
{
	for (auto it = sources_.rbegin(); it != sources_.rend(); ++it)
	{
		auto precompiled = (*it)->get_precompiled(shader_type);
		if (precompiled)
			return precompiled;
	}

	return {};
}

bool stack_source::has_source(GLenum shader_type) const
{
	return std::any_of(sources_.rbegin(), sources_.rend(),
					   [shader_type](const auto &source) { return source->has_source(shader_type); });
}

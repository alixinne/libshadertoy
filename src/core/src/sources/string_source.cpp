#include <algorithm>
#include <iterator>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/sources/string_source.hpp"

using namespace shadertoy::sources;

string_source::string_source(std::map<GLenum, std::string> strings)
{
	std::transform(strings.begin(), strings.end(), std::inserter(strings_, strings_.end()), [](const auto &pair) {
		return std::make_pair(pair.first, std::make_pair("<inline>", pair.second));
	});
}

string_source::string_source(std::map<GLenum, std::pair<std::string, std::string>> strings)
: strings_(std::move(strings))
{
}

std::vector<std::pair<std::string, std::string>> string_source::get_source(GLenum shader_type) const
{
	auto it = strings_.find(shader_type);
	if (it == strings_.end())
		throw shadertoy::shadertoy_error("unsupported shader type for this source");

	return { it->second };
}

bool string_source::has_source(GLenum shader_type) const
{
	return strings_.find(shader_type) != strings_.end();
}

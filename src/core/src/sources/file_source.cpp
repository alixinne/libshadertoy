#include <cstring>
#include <fstream>
#include <sstream>
#include <streambuf>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/sources/file_source.hpp"

using namespace shadertoy::sources;

file_source::file_source(std::map<GLenum, std::string> files) : files_(std::move(files)) {}

std::vector<std::pair<std::string, std::string>> file_source::get_source(GLenum shader_type) const
{
	auto it = files_.find(shader_type);
	if (it == files_.end())
		throw shadertoy::shadertoy_error("unsupported shader type for this source");

	std::string result;
	std::ifstream t(it->second);

	// Check file open status
	if (!t.is_open())
	{
		std::stringstream ss;
		ss << "failed to open " << it->second << ": " << strerror(errno);
		throw shadertoy::shadertoy_error(ss.str());
	}

	// Reserve space
	t.seekg(0, std::ios::end);
	result.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	// Load file
	result.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	return { { it->second, result } };
}

bool file_source::has_source(GLenum shader_type) const
{
	return files_.find(shader_type) != files_.end();
}

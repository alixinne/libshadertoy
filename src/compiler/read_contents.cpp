#include <fstream>
#include <sstream>

#include "shadertoy/compiler/template_error.hpp"

#include "read_contents.hpp"

using namespace shadertoy::compiler;

std::string read_contents(const std::string &filename)
{
	std::ifstream src(filename);

	if (!src.is_open())
	{
		std::stringstream ss;
		ss << "Failed to open file " << filename;
		throw template_error(ss.str());
	}

	std::string loaded_source;
	loaded_source.assign(std::istreambuf_iterator<char>(src),
						 std::istreambuf_iterator<char>());

	if (loaded_source.empty() || loaded_source.back() != '\n')
	{
		loaded_source += "\n";
	}

	src.close();
	
	return loaded_source;
}


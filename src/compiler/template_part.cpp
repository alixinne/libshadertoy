#include <fstream>
#include <sstream>

#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/utils/log.hpp"

using namespace shadertoy::compiler;
using namespace shadertoy::utils;

template_part::template_part(const std::string &name)
	: cloneable_part(name),
	sources_(),
	has_sources_(false)
{
}

template_part::template_part(const std::string &name, const std::string &source)
	: cloneable_part(name),
	sources_{ std::make_pair(name, source) },
	has_sources_(true)
{
}

template_part::template_part(const std::string &name, const std::vector<std::pair<std::string, std::string>> &sources)
	: cloneable_part(name),
	sources_(sources),
	has_sources_(true)
{
}

static std::string read_contents(const std::string &filename)
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
		loaded_source += "\n";

	src.close();
	
	return loaded_source;
}

template_part template_part::from_file(const std::string &name, const std::string &filename)
{
	std::vector<std::pair<std::string, std::string>> sources;
	sources.emplace_back(filename, read_contents(filename));
	log::shadertoy()->info("Loaded {}", filename);

	return template_part(name, sources);
}

template_part template_part::from_files(const std::string &name, const std::vector<std::string> &filenames)
{
	std::vector<std::pair<std::string, std::string>> sources;

	for (auto filename : filenames)
	{
		sources.emplace_back(filename, read_contents(filename));
		log::shadertoy()->info("Loaded {}", filename);
	}

	return template_part(name, sources);
}

template_part::operator bool() const
{
	return has_sources_;
}

std::vector<std::pair<std::string, std::string>> template_part::sources() const
{
	if (!has_sources_)
		throw template_error(std::string("Template part ") + name() + std::string(" is not specified"));
	return sources_;
}


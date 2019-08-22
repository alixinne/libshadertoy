#include <utility>

#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/utils/assert.hpp"

#include "read_contents.hpp"

using namespace shadertoy::compiler;
using namespace shadertoy::utils;

template_part::template_part(const std::string &name)
: cloneable_part(name),
  has_sources_(false)
{
}

template_part::template_part(const std::string &name, const std::string &source)
	: cloneable_part(name),
	sources_{ std::make_pair(name, source) },
	has_sources_(true)
{
}

template_part::template_part(const std::string &name, std::vector<std::pair<std::string, std::string>> sources)
: cloneable_part(name), sources_(std::move(sources)), has_sources_(true)
{
}

template_part template_part::from_file(const std::string &name, const std::string &filename)
{
	std::vector<std::pair<std::string, std::string>> sources;
	sources.emplace_back(filename, read_contents(filename));

	return template_part(name, sources);
}

template_part template_part::from_files(const std::string &name, const std::vector<std::string> &filenames)
{
	std::vector<std::pair<std::string, std::string>> sources;

	sources.reserve(filenames.size());
	for (auto filename : filenames)
	{
		sources.emplace_back(filename, read_contents(filename));
	}

	return template_part(name, sources);
}

template_part::operator bool() const
{
	return has_sources_;
}

std::vector<std::pair<std::string, std::string>> template_part::sources() const
{
	throw_assert<template_error>(has_sources_, "Template part {} is not specified", name());
	return sources_;
}


#include <algorithm>
#include <map>

#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/shader_template.hpp"

using namespace shadertoy::compiler;

shader_template::shader_template(std::deque<template_part> parts)
	: parts_(parts)
{
}

shader_template::shader_template()
	: parts_()
{
}

shader_template::shader_template(std::initializer_list<template_part> parts)
	: parts_()
{
	for (const auto &part : parts)
		add_part(part);
}

std::vector<std::pair<std::string, std::string>> shader_template::sources() const
{
	std::vector<std::pair<std::string, std::string>> result;
	result.reserve(parts_.size());

	for (auto &part : parts_)
	{
		std::copy(part.sources().begin(), part.sources().end(),
				  std::back_inserter(result));
	}

	return result;
}

shader_template shader_template::specify(std::initializer_list<template_part> parts) const
{
	// Create a map of new parts
	std::map<std::string, template_part> new_parts;
	for (const auto &part : parts)
		new_parts.emplace(part.name(), part);

	// New list of parts
	std::deque<template_part> specified_parts;

	for (auto &current_part : parts_)
	{
		if (!current_part)
		{
			// current_part is not specified yet
			auto new_part_it = new_parts.find(current_part.name());

			if (new_part_it != new_parts.end())
			{
				// We have a specification for this part
				specified_parts.push_back(new_part_it->second);
			}
			else
			{
				// Keep the unspecified part
				specified_parts.push_back(current_part);
			}
		}
		else
		{
			// This part has been specified already
			specified_parts.push_back(current_part);
		}
	}

	return shader_template(specified_parts);
}

void shader_template::add_part(const template_part &part)
{
	auto it = std::find_if(parts_.begin(), parts_.end(),
						   [&part](const auto &item)
						   { return item.name() == part.name(); });

	if (it != parts_.end())
	{
		throw template_error(std::string("A part named ") + part.name() + std::string(" already exists"));
	}

	parts_.push_back(part);
}

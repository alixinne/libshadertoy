#include <algorithm>
#include <map>
#include <sstream>

#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/shader_template.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy::compiler;

using shadertoy::utils::throw_assert;

shader_template::shader_template(std::deque<std::unique_ptr<basic_part>> parts)
	: parts_(std::move(parts))
{
}

void shader_template::check_unique(const std::unique_ptr<basic_part> &part)
{
	auto it = std::find_if(parts_.begin(), parts_.end(),
						   [&part](const auto &item)
						   { return item->name() == part->name(); });

	throw_assert<template_error>(it == parts_.end(),
								 "A part named {} already exists",
								 part->name());
}

shader_template::shader_template()
	: parts_()
{
}

shader_template::shader_template(shader_template &&other)
	: parts_(std::move(other.parts_))
{
}

shader_template &shader_template::operator=(shader_template &&rhs)
{
	if (this != &rhs)
	{
		parts_ = std::move(rhs.parts_);
	}
	return (*this);
}

std::vector<std::pair<std::string, std::string>> shader_template::sources() const
{
	std::vector<std::pair<std::string, std::string>> result;
	result.reserve(parts_.size());

	for (auto &part : parts_)
	{
		auto src(part->sources());
		std::copy(src.begin(), src.end(), std::back_inserter(result));
	}

	return result;
}

std::unique_ptr<basic_part> &shader_template::find(const std::string &name)
{
	for (auto &part : parts_)
	{
		if (part->name() == name)
		{
			return part;
		}
	}

	throw template_error(fmt::format("A part named {} could not be found", name));
}

shader_template shader_template::specify(std::vector<std::unique_ptr<basic_part>> parts) const
{
	// Create a map of new parts
	std::map<std::string, std::unique_ptr<basic_part>*> new_parts;
	for (auto &part : parts)
		new_parts.emplace(part->name(), &part);

	// New list of parts
	std::deque<std::unique_ptr<basic_part>> specified_parts;

	for (auto &current_part : parts_)
	{
		if (!current_part->specified())
		{
			// current_part is not specified yet
			auto new_part_it = new_parts.find(current_part->name());

			if (new_part_it != new_parts.end())
			{
				// We have a specification for this part
				specified_parts.emplace_back(std::move(*new_part_it->second));
			}
			else
			{
				// Keep the unspecified part
				specified_parts.emplace_back(current_part->clone());
			}
		}
		else
		{
			// This part has been specified already
			specified_parts.emplace_back(current_part->clone());
		}
	}

	return shader_template(std::move(specified_parts));
}

void shader_template::push_back(std::unique_ptr<basic_part> part)
{
	check_unique(part);
	parts_.emplace_back(std::move(part));
}

void shader_template::replace(const std::string &name, std::unique_ptr<basic_part> part)
{
	auto target_it = std::find_if(parts_.begin(), parts_.end(), [&name](const auto &item)
								  { return item->name() == name; });

	throw_assert<template_error>(target_it != parts_.end(),
								 "A part named {} could not be found for replacement",
								 name);

	auto duplicate = std::find_if(parts_.begin(), parts_.end(), [&part](const auto &item)
								  { return item->name() == part->name(); });

	throw_assert<template_error>(duplicate == parts_.end() || duplicate == target_it,
								 "A part named {} already exists",
								 part->name());

	*target_it = std::move(part);
}

void shader_template::insert_before(const std::string &target, std::unique_ptr<basic_part> part)
{
	check_unique(part);

	auto it = std::find_if(parts_.begin(), parts_.end(),
						   [&target](const auto &item)
						   { return item->name() == target; });

	throw_assert<template_error>(it != parts_.end(),
								 "A part named {} could not be found",
								 target);

	parts_.insert(it, std::move(part));
}

void shader_template::insert_after(const std::string &target, std::unique_ptr<basic_part> part)
{
	check_unique(part);

	auto it = std::find_if(parts_.begin(), parts_.end(),
						   [&target](const auto &item)
						   { return item->name() == target; });

	throw_assert<template_error>(it != parts_.end(),
								 "A part named {} could not be found",
								 target);

	parts_.insert(++it, std::move(part));
}

bool shader_template::erase(const std::string &name)
{
	auto it = std::find_if(parts_.begin(), parts_.end(), [&name](const auto &item)
						   { return item->name() == name; });

	if (it != parts_.end())
	{
		parts_.erase(it);
		return true;
	}

	return false;
}

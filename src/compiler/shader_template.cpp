#include <algorithm>
#include <map>
#include <sstream>

#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/shader_template.hpp"

using namespace shadertoy::compiler;

void shader_template::check_unique(jbcoe::polymorphic_value<basic_part> part)
{
	auto it = std::find_if(parts_.begin(), parts_.end(),
						   [&part](const auto &item)
						   { return item->name() == part->name(); });

	if (it != parts_.end())
	{
		throw template_error(std::string("A part named ") + part->name() + std::string(" already exists"));
	}
}

shader_template::shader_template(std::deque<jbcoe::polymorphic_value<basic_part>> parts)
	: parts_(parts)
{
}

shader_template::shader_template()
	: parts_()
{
}

shader_template::shader_template(std::initializer_list<jbcoe::polymorphic_value<basic_part>> parts)
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
		auto src(part->sources());
		std::copy(src.begin(), src.end(), std::back_inserter(result));
	}

	return result;
}

shader_template shader_template::specify(std::initializer_list<jbcoe::polymorphic_value<basic_part>> parts) const
{
	// Create a map of new parts
	std::map<std::string, jbcoe::polymorphic_value<basic_part>> new_parts;
	for (const auto &part : parts)
		new_parts.emplace(part->name(), part);

	// New list of parts
	std::deque<jbcoe::polymorphic_value<basic_part>> specified_parts;

	for (auto &current_part : parts_)
	{
		if (!current_part->specified())
		{
			// current_part is not specified yet
			auto new_part_it = new_parts.find(current_part->name());

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

void shader_template::add_part(jbcoe::polymorphic_value<basic_part> part)
{
	check_unique(part);
	parts_.push_back(part);
}

void shader_template::insert_before(jbcoe::polymorphic_value<basic_part> part, const std::string &target)
{
	check_unique(part);

	auto it = std::find_if(parts_.begin(), parts_.end(),
						   [&target](const auto &item)
						   { return item->name() == target; });

	if (it == parts_.end())
	{
		std::stringstream ss;
		ss << "A part named " << target << " could not be found";
		throw template_error(ss.str());
	}

	parts_.insert(it, part);
}

void shader_template::insert_after(jbcoe::polymorphic_value<basic_part> part, const std::string &target)
{
	check_unique(part);

	auto it = std::find_if(parts_.begin(), parts_.end(),
						   [&target](const auto &item)
						   { return item->name() == target; });

	if (it == parts_.end())
	{
		std::stringstream ss;
		ss << "A part named " << target << " could not be found";
		throw template_error(ss.str());
	}

	parts_.insert(++it, part);
}

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>
#include <regex>
#include <sstream>

#include "shadertoy/compiler/shader_template.hpp"
#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/template_part.hpp"

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

shader_template::shader_template() = default;

shader_template::shader_template(shader_template &&other) noexcept
	: parts_(std::move(other.parts_))
{
}

shader_template &shader_template::operator=(shader_template &&rhs) noexcept
{
	if (this != &rhs)
	{
		parts_ = std::move(rhs.parts_);
	}
	return (*this);
}

bool shader_template::is_specified() const
{
	return std::all_of(parts_.begin(), parts_.end(),
					   [](const auto &part_ptr) { return part_ptr->is_specified(); });
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

shader_template shader_template::parse(const std::string &source, const std::string &name)
{
	std::istringstream iss(source);
	return parse(iss, name);
}

static void emit_part(shader_template &st, std::string &name, std::ostringstream &ss)
{
	st.push_back(std::make_unique<template_part>(name, ss.str()));
	ss = std::ostringstream();
	name = std::string();
}

static void emit_part(shader_template &st, const std::string &name)
{
	st.push_back(std::make_unique<template_part>(name));
}

static void emit_part(shader_template &st, const std::string &name_prefix, int &name, std::ostringstream &ss)
{
	auto contents(ss.str());
	if (!contents.empty())
	{
		st.push_back(std::make_unique<template_part>(name_prefix + "-" + std::to_string(name++), contents));
		ss = std::ostringstream();
	}
}

shader_template shader_template::parse(std::istream &is, const std::string &name_prefix)
{
	const std::regex cmd_regex(
	R"(^\s*#pragma\s+shadertoy\s+part\s+(?:((?:\*|[a-zA-Z_][a-zA-Z\-_0-9]*):[a-zA-Z_][a-zA-Z\-_0-9]*)(?:\s+(begin))?|(end))\s*$)");

	enum
	{
		TP_GLSL_SOURCE,
		TP_TEMPLATE_SOURCE,
	} t_state = TP_GLSL_SOURCE;

	std::string line;
	std::ostringstream ss_current;
	std::string template_name;
	int part_number = 0;
	int line_num = 1;
	shader_template st;

	for (; std::getline(is, line); line_num++)
	{
		std::smatch match;

		if (std::regex_match(line, match, cmd_regex))
		{
			auto name = match[1].str();
			if (match[2].length() == 0)
			{
				// part name, no begin, or end
				if (match[3].str() == "end")
				{
					switch (t_state)
					{
					case TP_GLSL_SOURCE:
					{
						std::ostringstream ss;
						ss << line_num << ": unmatched end";
						throw template_error(ss.str());
					}
					break;
					case TP_TEMPLATE_SOURCE:
						emit_part(st, template_name, ss_current);
						t_state = TP_GLSL_SOURCE;
						break;
					}
				}
				else
				{
					std::string contents;
					switch (t_state)
					{
					case TP_GLSL_SOURCE:
						emit_part(st, name_prefix, part_number, ss_current);
						emit_part(st, name);
						break;
					case TP_TEMPLATE_SOURCE:
					{
						std::ostringstream ss;
						ss << line_num << ": unexpected template slot " << std::quoted(name);
						throw template_error(ss.str());
					}
					break;
					}
				}
			}
			else
			{
				switch (t_state)
				{
				case TP_GLSL_SOURCE:
					emit_part(st, name_prefix, part_number, ss_current);
					template_name = name;
					t_state = TP_TEMPLATE_SOURCE;
					break;
				case TP_TEMPLATE_SOURCE:
				{
					std::ostringstream ss;
					ss << line_num << ": unexpected template begin for " << std::quoted(name);
					throw template_error(ss.str());
				}
				break;
				}
			}
		}
		else
		{
			ss_current << line << std::endl;
		}
	}

	switch (t_state)
	{
	case TP_GLSL_SOURCE:
		emit_part(st, name_prefix, part_number, ss_current);
		break;
	case TP_TEMPLATE_SOURCE:
	{
		std::ostringstream ss;
		ss << line_num << ": unexpected end of file while looking for end part " << std::quoted(template_name);
		throw template_error(ss.str());
	}
	break;
	}

	return st;
}

shader_template shader_template::parse_file(const std::string &filename)
{
	std::ifstream ifs(filename);
	return parse(ifs, filename);
}

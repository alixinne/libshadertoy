#include "shadertoy/backends/gx/backend.hpp"

#include <set>
#include <sstream>

#include "shadertoy/backends/gx/program.hpp"

#include "shadertoy/compiler/program_template.hpp"
#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/shader_compiler.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::compiler;
using shadertoy::utils::log;
using shadertoy::utils::throw_assert;

shader_template program_template::specify_template_parts(const std::vector<std::unique_ptr<basic_part>> &parts,
														 const shader_template &source_template) const
{
	return source_template.specify_parts(
	parts, [&](const std::string &part_name) -> std::vector<std::unique_ptr<compiler::basic_part>> {
		auto sep = part_name.find(':');
		std::vector<std::unique_ptr<compiler::basic_part>> result;

		if (sep != std::string::npos)
		{
			std::string type_name(part_name.begin() + sep + 1, part_name.end());
			std::string subpart_name(part_name.begin(), part_name.begin() + sep);

			if (type_name == "defines")
			{
				if (subpart_name == "*")
				{
					std::transform(shader_defines_.begin(), shader_defines_.end(),
								   std::back_inserter(result), [](const auto &pair) {
									   return std::make_unique<compiler::define_part>(pair.first,
																					  pair.second);
								   });
				}
				else
				{
					auto it(shader_defines_.find(subpart_name));
					if (it != shader_defines_.end())
					{
						result.emplace_back(std::make_unique<compiler::define_part>(part_name, it->second));
					}
				}
			}
		}

		return result;
	});
}

std::vector<std::pair<std::string, std::string>>
program_template::get_source(const shader_template &shader_template,
							 const std::vector<std::unique_ptr<basic_part>> &parts) const
{
	auto specified_template = specify_template_parts(parts, shader_template);
	auto sources(specified_template.sources());

	if (log::shadertoy()->level() <= spdlog::level::trace)
	{
		std::stringstream ss;
		for (auto &pair : sources)
		{
			ss << pair.second;
		}
		log::shadertoy()->trace("Compiled following code for {}:\n{}",
								static_cast<const void *>(this), ss.str());
	}

	return sources;
}

program_template::program_template() = default;

program_template::program_template(std::map<GLenum, shader_template> shader_templates)
: shader_templates_(std::move(shader_templates))
{
}

bool program_template::emplace(GLenum type, shader_template &&shader_template)
{
	return shader_templates_.emplace(type, std::move(shader_template)).second;
}

std::vector<std::pair<std::string, std::string>>
program_template::get_source(GLenum type, const std::vector<std::unique_ptr<basic_part>> &parts) const
{
	auto it = shader_templates_.find(type);
	throw_assert<template_error>(it != shader_templates_.end(), "Shader type {} not found in program_template {}",
								 type, static_cast<const void *>(this));

	return get_source(it->second, parts);
}

std::shared_ptr<backends::gx::shader> program_template::get_precompiled(GLenum type) const
{
	auto it = compiled_shaders_.find(type);
	if (it == compiled_shaders_.end())
		return {};
	return it->second;
}

void program_template::compile(GLenum type)
{
	auto it = shader_templates_.find(type);

	throw_assert<template_error>(it != shader_templates_.end(), "Shader type {} not found in program_template {}",
								 type, static_cast<const void *>(this));

	// Create shader object
	auto so(backends::current()->make_shader(type));

	// Get sources
	auto sources(get_source(it->second, {}));

	// Compile shader
	shader_compiler::compile(*so, sources);

	// Compilation succeeded, add to cache
	compiled_shaders_.erase(type);
	compiled_shaders_.emplace(type, std::move(so));
}

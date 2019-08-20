#include <epoxy/gl.h>

#include <set>
#include <sstream>

#include "shadertoy/gl/program.hpp"

#include "shadertoy/compiler/program_template.hpp"
#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/shader_compiler.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::compiler;
using shadertoy::utils::throw_assert;
using shadertoy::utils::log;

shader_template program_template::specify_template_parts(const shader_template &source_template) const
{
	std::vector<std::unique_ptr<basic_part>> parts;
	return specify_template_parts(std::move(parts), source_template);
}

shader_template program_template::specify_template_parts(std::vector<std::unique_ptr<basic_part>> parts, const shader_template &source_template) const
{
	return source_template.specify_parts(
	std::move(parts), [&](const std::string &part_name) -> std::vector<std::unique_ptr<compiler::basic_part>> {
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

program_template::program_template() = default;

program_template::program_template(std::map<GLenum, shader_template> shader_templates)
: shader_templates_(std::move(shader_templates))
{
}

bool program_template::emplace(GLenum type, shader_template &&shader_template)
{
	return shader_templates_.emplace(type, std::move(shader_template)).second;
}

void program_template::compile(GLenum type)
{
	auto it = shader_templates_.find(type);

	throw_assert<template_error>(it != shader_templates_.end(), "Shader type {} not found in program_template {}",
								 type, static_cast<const void *>(this));

	// Create shader object
	gl::shader so(type);

	// Get sources
	auto sources(specify_template_parts(it->second).sources());
	
	if (log::shadertoy()->level() <= spdlog::level::trace)
	{
		std::stringstream ss;
		for (auto &pair : sources)
		{
			ss << pair.second;
		}
		log::shadertoy()->trace("Compiled following code for {}:\n{}", static_cast<const void *>(this), ss.str());
	}

	// Compile shader
	shader_compiler::compile(so, sources);

	// Compilation succeeded, add to cache
	compiled_shaders_.erase(type);
	compiled_shaders_.emplace(type, std::move(so));
}

gl::program program_template::compile(GLenum stage,
									  std::map<GLenum, std::vector<std::unique_ptr<basic_part>>> parts,
									  std::map<GLenum, std::string> *compiled_sources) const
{
	std::set<GLenum> allowed_shaders;

	if (stage == GL_FRAGMENT_SHADER)
	{
		allowed_shaders.insert(GL_VERTEX_SHADER);
		allowed_shaders.insert(GL_FRAGMENT_SHADER);
	}
	else if (stage == GL_COMPUTE_SHADER)
	{
		allowed_shaders.insert(GL_COMPUTE_SHADER);
	}
	else
	{
		throw shadertoy_error("invalid shader stage");
	}

	gl::program program;

	std::vector<gl::shader> attached_shaders;

	// Compile and attach fully specified shaders
	for (const auto &pair : shader_templates_)
	{
		if (allowed_shaders.find(pair.first) == allowed_shaders.end())
			continue;

		// Do not try to recompile precompiled shaders
		if (compiled_shaders_.find(pair.first) != compiled_shaders_.end())
		{
			continue;
		}

		// Compile sources
		std::vector<std::pair<std::string, std::string>> sources;

		// Add parts given as argument
		auto &specified_template = pair.second;
		auto it = parts.find(pair.first);

		// Get sources
		if (it != parts.end())
		{
			sources = specify_template_parts(std::move(it->second), specified_template).sources();
		}
		else
		{
			sources = specified_template.sources();
		}

		if (log::shadertoy()->level() <= spdlog::level::trace || compiled_sources != nullptr)
		{
			std::stringstream ss;
			for (auto &pair : sources)
			{
				ss << pair.second;
			}

			auto result(ss.str());

			if (compiled_sources != nullptr)
			{
				compiled_sources->emplace(pair.first, result);
			}

			log::shadertoy()->trace("Compiled following code for {}:\n{}", static_cast<const void *>(this), result);
		}

		// Compile shader
		gl::shader so(pair.first);
		shader_compiler::compile(so, sources);

		// Add shader for attachment
		attached_shaders.emplace_back(std::move(so));
	}

	// Attach pre-compiled shaders
	for (const auto &pair : compiled_shaders_)
	{
		if (allowed_shaders.find(pair.first) == allowed_shaders.end())
			continue;

		program.attach_shader(pair.second);
	}

	// Attach compiled on-the-fly shaders
	for (const auto &s : attached_shaders)
	{
		program.attach_shader(s);
	}

	try
	{
		// Link program
		program.link();
	}
	catch (const shadertoy::gl::program_link_error &ex)
	{
		// Detach shaders
		for (const auto &s : attached_shaders)
		{
			program.detach_shader(s);
		}

		for (const auto &pair : compiled_shaders_)
		{
			if (allowed_shaders.find(pair.first) == allowed_shaders.end())
				continue;

			program.detach_shader(pair.second);
		}

		throw;
	}

	// Detach shaders
	for (const auto &s : attached_shaders)
	{
		program.detach_shader(s);
	}

	for (const auto &pair : compiled_shaders_)
	{
		if (allowed_shaders.find(pair.first) == allowed_shaders.end())
			continue;

		program.detach_shader(pair.second);
	}

	return program;
}

gl::program program_template::compile(GLenum stage, const std::map<GLenum, shader_template> &templates, std::map<GLenum, std::string> *compiled_sources) const
{
	std::set<GLenum> allowed_shaders;

	if (stage == GL_FRAGMENT_SHADER)
	{
		allowed_shaders.insert(GL_VERTEX_SHADER);
		allowed_shaders.insert(GL_FRAGMENT_SHADER);
	}
	else if (stage == GL_COMPUTE_SHADER)
	{
		allowed_shaders.insert(GL_COMPUTE_SHADER);
	}
	else
	{
		throw shadertoy_error("invalid shader stage");
	}

	gl::program program;

	std::vector<gl::shader> attached_shaders;

	// Compile and attach templates
	for (const auto &pair : templates)
	{
		if (allowed_shaders.find(pair.first) == allowed_shaders.end())
			continue;

		// Compile sources
		auto sources(pair.second.sources());

		if (log::shadertoy()->level() <= spdlog::level::trace || compiled_sources != nullptr)
		{
			std::stringstream ss;
			for (auto &pair : sources)
			{
				ss << pair.second;
			}

			auto result(ss.str());

			if (compiled_sources != nullptr)
			{
				compiled_sources->emplace(pair.first, result);
			}

			log::shadertoy()->trace("Compiled following code for {}:\n{}",
									(void*)this,
									result);
		}

		// Compile shader
		gl::shader so(pair.first);
		shader_compiler::compile(so, sources);

		// Add shader for attachment
		attached_shaders.emplace_back(std::move(so));
	}

	// Attach pre-compiled shaders
	for (const auto &pair : compiled_shaders_)
	{
		if (allowed_shaders.find(pair.first) == allowed_shaders.end())
			continue;

		// Only attach shaders that are not being overriden
		if (templates.find(pair.first) == templates.end())
		{
			program.attach_shader(pair.second);
		}
	}

	// Attach compiled on-the-fly shaders
	for (const auto &s : attached_shaders)
	{
		program.attach_shader(s);
	}

	try
	{
		// Link program
		program.link();
	}
	catch (const shadertoy::gl::program_link_error &ex)
	{
		// Detach shaders
		for (const auto &s : attached_shaders)
		{
			program.detach_shader(s);
		}

		for (const auto &pair : compiled_shaders_)
		{
			if (allowed_shaders.find(pair.first) == allowed_shaders.end())
				continue;

			if (templates.find(pair.first) == templates.end())
			{
				program.detach_shader(pair.second);
			}
		}

		throw;
	}

	// Detach shaders
	for (const auto &s : attached_shaders)
	{
		program.detach_shader(s);
	}

	for (const auto &pair : compiled_shaders_)
	{
		if (allowed_shaders.find(pair.first) == allowed_shaders.end())
			continue;

		if (templates.find(pair.first) == templates.end())
		{
			program.detach_shader(pair.second);
		}
	}

	return program;
}

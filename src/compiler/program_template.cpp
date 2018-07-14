#include <sstream>
#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/compiler/program_template.hpp"
#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/shader_compiler.hpp"
#include "shadertoy/uniform_state.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::compiler;
using shadertoy::utils::throw_assert;
using shadertoy::utils::log;

program_template::program_template()
	: shader_templates_(),
	compiled_shaders_()
{
}

program_template::program_template(std::map<GLenum, shader_template> shader_templates)
	: shader_templates_(std::move(shader_templates)),
	compiled_shaders_()
{
}

bool program_template::emplace(GLenum type, shader_template &&shader_template)
{
	return shader_templates_.emplace(type, std::move(shader_template)).second;
}

void program_template::compile(GLenum type)
{
	auto it = shader_templates_.find(type);
	
	throw_assert<template_error>(it != shader_templates_.end(),
								 "Shader type {} not found in program_template {}",
								 type, (void*)this);

	// Create shader object
	gl::shader so(type);

	// Get sources
	auto sources(it->second.sources());
	
	if (log::shadertoy()->level() <= spdlog::level::debug)
	{
		std::stringstream ss;
		for (auto &pair : sources)
			ss << pair.second;
		log::shadertoy()->debug("Compiled following code for {}:\n{}",
								(void*)this,
								ss.str());
	}

	// Compile shader
	shader_compiler::compile(so, sources);

	// Compilation succeeded, add to cache
	compiled_shaders_.erase(type);
	compiled_shaders_.emplace(type, std::move(so));
}

gl::program program_template::compile(std::map<GLenum, std::vector<std::unique_ptr<basic_part>>> parts) const
{
	gl::program program;

	std::vector<gl::shader> attached_shaders;

	// Compile and attach fully specified shaders
	for (const auto &pair : shader_templates_)
	{
		// Do not try to recompile precompiled shaders
		if (compiled_shaders_.find(pair.first) != compiled_shaders_.end())
			continue;

		// Compile sources
		std::vector<std::pair<std::string, std::string>> sources;

		// Add parts given as argument
		auto &specified_template = pair.second;
		auto it = parts.find(pair.first);

		// Get sources
		if (it != parts.end())
		{
			sources = specified_template.specify_parts(std::move(it->second), [&](const std::string &part_name)
				-> std::vector<std::unique_ptr<compiler::basic_part>> {
				auto sep = part_name.find(":");
				std::vector<std::unique_ptr<compiler::basic_part>> result;

				if (sep != std::string::npos)
				{
					std::string type_name(part_name.begin() + sep + 1, part_name.end());
					std::string subpart_name(part_name.begin(), part_name.begin() + sep);

					if (type_name == "uniforms")
					{
						if (subpart_name == "*")
						{
							std::transform(shader_inputs_.begin(), shader_inputs_.end(),
										   std::back_inserter(result), [](const auto &pair) {
											   return std::make_unique<compiler::template_part>(
											   pair.first, pair.second->definitions_string());
										   });
						}
						else
						{
							auto it(shader_inputs_.find(subpart_name));
							if (it != shader_inputs_.end())
							{
								result.emplace_back(std::make_unique<compiler::template_part>(part_name, it->second->definitions_string()));
							}
						}
					}
					else if (type_name == "defines")
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
			}).sources();
		}
		else
		{
			sources = specified_template.sources();
		}

		if (log::shadertoy()->level() <= spdlog::level::debug)
		{
			std::stringstream ss;
			for (auto &pair : sources)
				ss << pair.second;
			log::shadertoy()->debug("Compiled following code for {}:\n{}",
									(void*)this,
									ss.str());
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
			program.detach_shader(s);

		for (const auto &pair : compiled_shaders_)
			program.detach_shader(pair.second);

		throw;
	}

	// Detach shaders
	for (const auto &s : attached_shaders)
		program.detach_shader(s);

	for (const auto &pair : compiled_shaders_)
		program.detach_shader(pair.second);

	return program;
}

gl::program program_template::compile(const std::map<GLenum, shader_template> &templates) const
{
	gl::program program;

	std::vector<gl::shader> attached_shaders;

	// Compile and attach templates
	for (const auto &pair : templates)
	{
		// Compile sources
		auto sources(pair.second.sources());

		if (log::shadertoy()->level() <= spdlog::level::debug)
		{
			std::stringstream ss;
			for (auto &pair : sources)
				ss << pair.second;
			log::shadertoy()->debug("Compiled following code for {}:\n{}",
									(void*)this,
									ss.str());
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
		// Only attach shaders that are not being overriden
		if (templates.find(pair.first) == templates.end())
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
			program.detach_shader(s);

		for (const auto &pair : compiled_shaders_)
		{
			if (templates.find(pair.first) == templates.end())
				program.detach_shader(pair.second);
		}

		throw;
	}

	// Detach shaders
	for (const auto &s : attached_shaders)
		program.detach_shader(s);

	for (const auto &pair : compiled_shaders_)
	{
		if (templates.find(pair.first) == templates.end())
			program.detach_shader(pair.second);
	}

	return program;
}

std::vector<std::unique_ptr<bound_inputs_base>> program_template::bind_inputs(const gl::program &program) const
{
	std::vector<std::unique_ptr<bound_inputs_base>> bound_inputs;

	for (const auto &input_object : shader_inputs_)
		bound_inputs.push_back(input_object.second->bind_inputs(program));

	return bound_inputs;
}

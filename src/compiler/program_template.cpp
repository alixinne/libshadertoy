#include <sstream>
#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/program_template.hpp"

#include "shadertoy/shader_compiler.hpp"

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
			sources = specified_template.specify(std::move(it->second)).sources();
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

#include "shadertoy/backends/gx/backend.hpp"
#include "shadertoy/shader_compiler.hpp"

#include "shadertoy/sources/basic_source.hpp"

using namespace shadertoy::backends;
using namespace shadertoy::sources;

std::shared_ptr<gx::shader> basic_source::get_precompiled(GLenum shader_type) const { return {}; }

std::shared_ptr<gx::shader> basic_source::compile_shader(GLenum shader_type) const
{
	if (auto precompiled = get_precompiled(shader_type))
		return precompiled;

	// Fetch sources
	auto sources(get_source(shader_type));
	// Create OpenGL shader object
	auto so(current()->make_shader(shader_type));

	// Compile shader
	shader_compiler::compile(*so, sources);

	// Return it as a shared_ptr
	return std::move(so);
}

std::unique_ptr<gx::program> basic_source::compile_program(const std::vector<GLenum> &shader_types) const
{
	std::vector<std::shared_ptr<gx::shader>> attached_shaders;

	// Compile shaders
	for (auto shader_type : shader_types)
	{
		attached_shaders.emplace_back(compile_shader(shader_type));
	}

	// Create program object
	auto program(current()->make_program());

	// Attach shaders
	for (const auto &shader : attached_shaders)
	{
		program->attach_shader(*shader);
	}

	try
	{
		// Link program
		program->link();
	}
	catch (...)
	{
		// Detach shaders
		for (const auto &shader : attached_shaders)
		{
			program->detach_shader(*shader);
		}

		throw;
	}

	// Detach shaders
	for (const auto &shader : attached_shaders)
	{
		program->detach_shader(*shader);
	}

	return program;
}

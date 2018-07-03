#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/program_buffer.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/compiler/input_part.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;
using shadertoy::gl::gl_call;

using shadertoy::utils::log;

program_buffer::program_buffer(const std::string &id)
	: gl_buffer(id),
	  bound_inputs_(),
	  inputs_(),
	  source_files_()
{
}

void program_buffer::init_contents(const render_context &context, const io_resource &io)
{
	// Initialize the geometry
	log::shadertoy()->trace("Loading geometry for {} ({})", id(), (void*)this);
	init_geometry(context, io);

	// Shader objects
	log::shadertoy()->trace("Compiling program for {} ({})", id(), (void*)this);

	// Load the fragment shader for this buffer
	std::vector<std::unique_ptr<compiler::basic_part>> fs_template_parts;

	// Add the uniform inputs for this buffer
	fs_template_parts.emplace_back(std::make_unique<compiler::input_part>("buffer:inputs", inputs_));
	fs_template_parts.emplace_back(std::make_unique<compiler::template_part>(compiler::template_part::from_files("buffer:sources", source_files_)));

	// Expensive log operation, only log if level is requiring it
	if (log::shadertoy()->level() <= spdlog::level::info && !source_files_.empty())
	{
		std::stringstream ss;
		for (auto &file : source_files_)
			ss << file << ";";
		auto str(ss.str());
		log::shadertoy()->info("Loaded {} for {} ({})",
							   std::string(str.begin(), str.end() - 1),
							   id(),
							   (void*)this);
	}

	// Compile
	std::map<GLenum, std::vector<std::unique_ptr<compiler::basic_part>>> parts;
	parts.emplace(GL_FRAGMENT_SHADER, std::move(fs_template_parts));

	auto &buffer_template(context.buffer_template());
	program_ = buffer_template.compile(std::move(parts));

	// Use the program
	program_.use();

	// bind uniform inputs
	bound_inputs_ = buffer_template.bind_inputs(program_);

	log::shadertoy()->debug("Bound {} input objects for {} ({})",
							bound_inputs_.size(),
							id(),
							(void*)this);

	// Set input uniform units
	size_t current_unit = 0;
	for (auto it = inputs_.begin(); it != inputs_.end(); ++it, ++current_unit)
	{
		auto location(program_.get_uniform_location(it->sampler_name().c_str()));
		location.set_value(static_cast<GLint>(current_unit));
	}
}

void program_buffer::render_gl_contents(const render_context &context, const io_resource &io)
{
	// Compute the rendering size
	rsize size(io.render_size()->resolve());

	// Set viewport
	gl_call(glViewport, 0, 0, size.width, size.height);

	// Prepare the render target
	gl_call(glClearColor, 0.f, 0.f, 0.f, 1.f);
	gl_call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup program and its uniforms
	program_.use();

	// Apply context-level uniforms
	for (auto &inputs : bound_inputs_)
        inputs->apply();

	// Override values in bound inputs 0 (ShaderToy inputs)
	auto &state(*static_cast<shader_inputs_t::bound_inputs*>(bound_inputs_[0].get()));
	std::array<glm::vec3, SHADERTOY_ICHANNEL_COUNT> resolutions(state.get<iChannelResolution>());

	// Setup the texture targets
	size_t current_unit = 0;
	for (auto it = inputs_.begin(); it != inputs_.end(); ++it, ++current_unit)
	{
		auto &input(it->input());
		glm::vec3 sz(0.f);

		if (input)
		{
			auto texture(input->bind(current_unit));

			texture->get_parameter(0, GL_TEXTURE_WIDTH, &sz.x);
			texture->get_parameter(0, GL_TEXTURE_HEIGHT, &sz.y);
			sz.z = 1.0f;
		}
		else
		{
			context.error_input()->bind(current_unit);
		}

		if (current_unit < SHADERTOY_ICHANNEL_COUNT)
			resolutions[current_unit] = sz;
	}

	state.set<iChannelResolution>(resolutions);

	// Set the current buffer resolution
	state.set<iResolution>(glm::vec3(size.width, size.height, 1.f));

	// Try to set iTimeDelta
	GLint available = 0;
	time_delta_query().get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
	if (available)
	{
		// Result available, set uniform value
		GLuint64 timeDelta;
		time_delta_query().get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
		state.set<iTimeDelta>(timeDelta / 1e9);
	}

	// Render the program
	render_geometry(context, io);
}


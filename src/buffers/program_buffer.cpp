#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/buffers/program_buffer.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/compiler/file_part.hpp"
#include "shadertoy/compiler/input_part.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;
using shadertoy::gl::gl_call;

using shadertoy::utils::log;

program_buffer::program_buffer(const std::string &id)
: gl_buffer(id),

  source_map_(nullptr)
{
}

void program_buffer::init_contents(const render_context &context, const io_resource &io)
{
	// Initialize the geometry
	log::shadertoy()->trace("Loading geometry for {} ({})", id(), static_cast<const void *>(this));
	init_geometry(context, io);

	// Shader objects
	log::shadertoy()->trace("Compiling program for {} ({})", id(), static_cast<const void *>(this));

	// Load the fragment shader for this buffer
	std::vector<std::unique_ptr<compiler::basic_part>> fs_template_parts;

	// Add the uniform inputs for this buffer
	fs_template_parts.emplace_back(std::make_unique<compiler::input_part>("buffer:inputs", inputs_));
	if (source_)
	{
		fs_template_parts.emplace_back(source_->clone());
	}

	// Compile
	std::map<GLenum, std::vector<std::unique_ptr<compiler::basic_part>>> parts;
	parts.emplace(GL_FRAGMENT_SHADER, std::move(fs_template_parts));

	const auto &buffer_template(override_program_ ? *override_program_ : context.buffer_template());
	program_ = buffer_template.compile(std::move(parts), source_map_);

	// Use the program
	program_.use();

	// bind uniform inputs
	bound_inputs_ = buffer_template.bind_inputs(program_);

	log::shadertoy()->debug("Bound {} input objects for {} ({})", bound_inputs_.size(), id(),
							static_cast<const void *>(this));

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
	// Compute the rendering size from the current viewport
	GLint viewport[4]; // x, y, width, height
	gl_call(glGetIntegerv, GL_VIEWPORT, &viewport[0]);
	rsize size(viewport[2], viewport[3]);

	// Setup program and its uniforms
	program_.use();

	// Apply context-level uniforms
	for (auto &inputs : bound_inputs_)
	{
		inputs->apply();
	}

	// Override values in bound inputs 0 (ShaderToy inputs)
	shader_inputs_t::bound_inputs *state_ptr = nullptr;
	for (auto &bound_input : bound_inputs_)
	{
		if ((state_ptr = dynamic_cast<shader_inputs_t::bound_inputs *>(bound_input.get())) != nullptr)
		{
			break;
		}
	}

	auto &state(*state_ptr);
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
		{
			resolutions[current_unit] = sz;
		}
	}

	state.set<iChannelResolution>(resolutions);

	// Set the current buffer resolution
	state.set<iResolution>(glm::vec3(size.width, size.height, 1.f));

	// Try to set iTimeDelta
	GLint available = 0;
	time_delta_query().get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
	if (available != 0)
	{
		// Result available, set uniform value
		GLuint64 timeDelta;
		time_delta_query().get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
		state.set<iTimeDelta>(timeDelta / 1e9);
	}

	// Render the program
	render_geometry(context, io);
}

void program_buffer::source(const std::string &new_source)
{
	source_ = std::unique_ptr<compiler::basic_part>(std::make_unique<compiler::template_part>("buffer:sources", new_source));
}

void program_buffer::source_file(const std::string &new_file)
{
	source_ = std::unique_ptr<compiler::basic_part>(std::make_unique<compiler::file_part>("buffer:sources", new_file));
}

std::optional<std::vector<buffer_output>> program_buffer::get_buffer_outputs() const
{
	GLint active_outputs;
	program_.get_program_interface(GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &active_outputs);

	std::vector<buffer_output> outputs;
	outputs.reserve(active_outputs);

	const GLenum properties[] = { GL_TYPE, GL_LOCATION, GL_NAME_LENGTH };
	constexpr const size_t propCount = sizeof(properties) / sizeof(properties[0]);
	for (int output = 0; output < active_outputs; ++output)
	{
		// Get properties
		GLint values[propCount];
		program_.get_program_resource(GL_PROGRAM_OUTPUT, output, propCount, std::begin(properties),
									  propCount, nullptr, std::begin(values));

		// Get name
		std::string output_name(values[2] - 1, ' ');
		program_.get_program_resource_name(GL_PROGRAM_OUTPUT, output, values[2], nullptr,
										   output_name.data());

		log::shadertoy()->debug("Discovered program output #{} layout(location = {}) {:#x} {}",
								output, values[1], values[0], output_name);

		outputs.emplace_back(output_name, values[1], values[0]);
	}

	return outputs;
}

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

	// Discover program interface
	program_interface_ = std::make_unique<program_interface>(program_);

	log::shadertoy()->debug("Program {} ({}) has {} uniform inputs",
							id(), static_cast<const void *>(this),
							program_interface_->uniforms().resources().size());

	// Set input uniform units
	size_t current_unit = 0;
	for (auto it = inputs_.begin(); it != inputs_.end(); ++it, ++current_unit)
	{
		if (auto resource = program_interface_->uniforms().try_get(it->sampler_name()))
		{
			resource->get_location(program_).set_value(static_cast<GLint>(current_unit));
		}
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

	// Set iChannelResolution details
	std::array<glm::vec3, SHADERTOY_ICHANNEL_COUNT> resolutions;

	// Setup the texture targets
	size_t current_unit = 0;
	for (auto it = inputs_.begin(); it != inputs_.end(); ++it, ++current_unit)
	{
		auto &input(it->input());
		glm::vec3 sz(0.f);

		// Set the sampler uniform value
		if (!it->sampler_name().empty())
		{
			if (auto sampler_uniform = program_interface_->try_get_uniform_location(it->sampler_name()))
			{
				sampler_uniform->set_value(static_cast<int>(current_unit));
			}
		}

		// Bind the texture to the unit
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

	if (auto channel_resolutions_resource = program_interface_->uniforms().try_get("iChannelResolution"))
	{
		channel_resolutions_resource->get_location(program_).set_value(resolutions.size(), resolutions.data());
	}

	// Set the current buffer resolution
	if (auto resolution_resource = program_interface_->uniforms().try_get("iResolution"))
	{
		resolution_resource->get_location(program_).set_value(glm::vec3(size.width, size.height, 1.f));
	}

	// Try to set iTimeDelta
	if (auto time_delta_resource = program_interface_->uniforms().try_get("iTimeDelta"))
	{
		GLint available = 0;
		time_delta_query().get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
		if (available != 0)
		{
			// Result available, set uniform value
			GLuint64 timeDelta;
			time_delta_query().get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
			time_delta_resource->get_location(program_).set_value(timeDelta / 1e9f);
		}
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
	std::vector<buffer_output> outputs;
	outputs.reserve(program_interface_->outputs().resources().size());

	for (const auto &output : program_interface_->outputs().resources())
	{
		log::shadertoy()->debug("Discovered program output #{} layout(location = {}) {:#x} {}",
								outputs.size(), output.location, output.type, output.name);

		outputs.emplace_back(output.name, output.location, output.type);
	}

	return outputs;
}

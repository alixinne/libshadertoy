#include <epoxy/gl.h>

#include "shadertoy/program_host.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/render_context.hpp"

#include "shadertoy/compiler/file_part.hpp"
#include "shadertoy/compiler/input_part.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/gl.hpp"
#include "shadertoy/gl/wrapper_context.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;

program_host::program_host() : source_map_(nullptr) {}

void program_host::init_program(const render_context &context)
{
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

void program_host::prepare_render(const render_context &context)
{
	// Setup program and its uniforms
	program_.use();

	// Set iChannelResolution details
	std::array<glm::vec3, SHADERTOY_ICHANNEL_COUNT> resolutions;

	// Setup the texture targets
	size_t current_texture_unit = 0,
		   current_image_unit = 0;
	for (auto it = inputs_.begin(); it != inputs_.end(); ++it)
	{
		auto &input(it->input());
		glm::vec3 sz(0.f);

		size_t *current_unit = nullptr;

		if (it->type() == program_input_type::sampler)
		{
			current_unit = &current_texture_unit;

			// Bind the texture to the unit
			if (input)
			{
				auto texture(input->bind(*current_unit));

				texture->get_parameter(0, GL_TEXTURE_WIDTH, &sz.x);
				texture->get_parameter(0, GL_TEXTURE_HEIGHT, &sz.y);
				sz.z = 1.0f;
			}
			else
			{
				context.error_input()->bind(*current_unit);
			}

			if (*current_unit < SHADERTOY_ICHANNEL_COUNT)
			{
				resolutions[*current_unit] = sz;
			}
		}
		else if (it->type() == program_input_type::image)
		{
			current_unit = &current_image_unit;

			if (input)
			{
				input->bind_image(*current_unit);
			}
		}

		// Skip setting sampler value if we have no unit number to set
		if (current_unit == nullptr) continue;

		// Set the sampler uniform value
		if (!it->sampler_name().empty())
		{
			if (auto sampler_uniform = program_interface_->try_get_uniform_location(it->sampler_name()))
			{
				sampler_uniform->set_value(static_cast<int>(*current_unit));
			}
		}

		// Increment unit
		(*current_unit)++;
	}

	// Unbind extra texture units not in use
	gl::current_context.unbind_texture_units(current_texture_unit);

	if (auto channel_resolutions_resource = program_interface_->uniforms().try_get("iChannelResolution"))
	{
		channel_resolutions_resource->get_location(program_).set_value(resolutions.size(), resolutions.data());
	}
}

void program_host::source(const std::string &new_source)
{
	source_ = std::unique_ptr<compiler::basic_part>(
	std::make_unique<compiler::template_part>("buffer:sources", new_source));
}

void program_host::source_file(const std::string &new_file)
{
	source_ =
	std::unique_ptr<compiler::basic_part>(std::make_unique<compiler::file_part>("buffer:sources", new_file));
}


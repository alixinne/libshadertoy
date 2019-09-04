#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/program_host.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/render_context.hpp"

#include "shadertoy/compiler/file_part.hpp"
#include "shadertoy/compiler/input_part.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;

program_host::program_host() {}

void program_host::init_program(const render_context &context, GLenum stage)
{
	if (stage != GL_FRAGMENT_SHADER && stage != GL_COMPUTE_SHADER)
		throw shadertoy_error("invalid shader stage");

	// Compile
	program_ = source_->compile_program(stage == GL_COMPUTE_SHADER ?
										std::vector<GLenum>{ GL_COMPUTE_SHADER } :
										std::vector<GLenum>{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER });

	// Discover program interface
	program_interface_ = std::make_unique<program_interface>(*program_);

	// Set input uniform units
	size_t current_unit = 0;
	for (auto it = inputs_.begin(); it != inputs_.end(); ++it, ++current_unit)
	{
		auto name = it->sampler_name();

		if (name.empty())
		{
			if (current_unit < SHADERTOY_ICHANNEL_COUNT)
			{
				name = "iChannel" + std::to_string(current_unit);
			}
			else
			{
				break;
			}
		}

		if (auto resource = program_interface_->try_get_uniform_location(name))
		{
			resource->set_value(static_cast<GLint>(current_unit));
		}
	}
}

void program_host::prepare_render(const render_context &context)
{
	// Setup program and its uniforms
	program_->use();

	// Set iChannelResolution details
	std::array<glm::vec3, SHADERTOY_ICHANNEL_COUNT> resolutions;

	// Setup the texture targets
	size_t current_texture_unit = 0;
#if SHADERTOY_HAS_IMAGE_LOAD_STORE
	size_t current_image_unit = 0;
#endif
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

				texture->get_parameter(GL_TEXTURE_WIDTH, &sz.x);
				texture->get_parameter(GL_TEXTURE_HEIGHT, &sz.y);
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
#if SHADERTOY_HAS_IMAGE_LOAD_STORE
		else if (it->type() == program_input_type::image)
		{
			current_unit = &current_image_unit;

			if (input)
			{
				input->bind_image(*current_unit);
			}
		}
#endif

		// Skip setting sampler value if we have no unit number to set
		if (current_unit == nullptr)
			continue;

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
	backends::current()->unbind_texture_units(current_texture_unit);

	if (auto channel_resolutions_resource = program_interface_->try_get_uniform_location("iChannelResolution"))
	{
		channel_resolutions_resource->set_value(resolutions.size(), resolutions.data());
	}
}

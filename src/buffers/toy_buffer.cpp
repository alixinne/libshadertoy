#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/toy_buffer.hpp"
#include "shadertoy/render_context.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;
using shadertoy::gl::gl_call;

toy_buffer::toy_buffer(const std::string &id)
	: gl_buffer(id),
	  fs_(GL_FRAGMENT_SHADER),
	  bound_inputs_(),
	  inputs_(),
	  source_files_()
{
}

void toy_buffer::init_contents(render_context &context, io_resource &io)
{
	// Attach the vertex shader for the screen quad
	program_.attach_shader(context.screen_quad_vertex_shader());

	// Load the fragment shader for this buffer
	context.build_buffer_shader(*this, fs_);

	// Attach shader
	program_.attach_shader(fs_);

	// Link the program
	program_.link();

	// Use the program
	program_.use();

	// bind uniform inputs
	bound_inputs_ = context.bound_inputs(program_);

	// Set input uniform units
	size_t current_unit = 0;
	for (auto it = inputs_.begin(); it != inputs_.end(); ++it, ++current_unit)
	{
		auto location(program_.get_uniform_location(it->sampler_name().c_str()));
		location.set_value(static_cast<GLint>(current_unit));
	}
}

void toy_buffer::render_gl_contents(render_context &context, io_resource &io)
{
	// Compute the rendering size
	rsize size(io.size());

	// Set viewport
	gl_call(glViewport, 0, 0, size.width, size.height);

	// Prepare the render target
	gl_call(glClearColor, 0.f, 0.f, 0.f, 1.f);
	gl_call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup program and its uniforms
	program_.use();

	// Override values in bound inputs 0 (ShaderToy inputs)
	auto &state(std::static_pointer_cast<shader_inputs_t::bound_inputs>(bound_inputs_[0])->state);
	auto &resolutions(state.get<iChannelResolution>());

	// Setup the texture targets
	size_t current_unit = 0;
	for (auto it = inputs_.begin(); it != inputs_.end(); ++it, ++current_unit)
	{
		auto &input(it->input());

		if (input)
		{
			auto texture(input->bind(current_unit));

			if (current_unit < SHADERTOY_ICHANNEL_COUNT)
			{
				texture->get_parameter(0, GL_TEXTURE_WIDTH, &resolutions[current_unit][0]);
				texture->get_parameter(0, GL_TEXTURE_HEIGHT, &resolutions[current_unit][1]);
				resolutions[current_unit][2] = 1.0f;
			}
		}
		else
		{
			context.error_input()->bind(current_unit);

			if (current_unit < SHADERTOY_ICHANNEL_COUNT)
			{
				resolutions[current_unit] = glm::vec3(0.f);
			}
		}
	}

	// Set the current buffer resolution
	state.get<iResolution>()[0] = static_cast<float>(size.width);
	state.get<iResolution>()[1] = static_cast<float>(size.height);

	// Try to set iTimeDelta
	GLint available = 0;
	time_delta_query().get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
	if (available)
	{
		// Result available, set uniform value
		GLuint64 timeDelta;
		time_delta_query().get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
		state.get<iTimeDelta>() = timeDelta / 1e9;
	}

	// Set all uniforms
	for (auto &inputs : bound_inputs_)
        inputs->apply();

	// Render the program
    context.render_screen_quad(time_delta_query());
}


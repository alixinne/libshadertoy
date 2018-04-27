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
	  bound_inputs_(),
	  inputs_(),
	  source_files_()
{
}

void toy_buffer::init_contents(const render_context &context, const io_resource &io)
{
	// Shader objects
	const gl::shader &vs(context.screen_quad_vertex_shader());
	gl::shader fs(GL_FRAGMENT_SHADER);

	// Load the fragment shader for this buffer
	context.build_buffer_shader(*this, fs);

	// Attach the shaders
	program_.attach_shader(vs);
	program_.attach_shader(fs);

	// Link the program
	program_.link();

	// Detach the shaders
	program_.detach_shader(vs);
	program_.detach_shader(fs);

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

void toy_buffer::render_gl_contents(const render_context &context, const io_resource &io)
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

	// Apply context-level uniforms
	for (auto &inputs : bound_inputs_)
        inputs->apply();

	// Override values in bound inputs 0 (ShaderToy inputs)
	auto &state(*std::static_pointer_cast<shader_inputs_t::bound_inputs>(bound_inputs_[0]));
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
    context.render_screen_quad(time_delta_query());
}


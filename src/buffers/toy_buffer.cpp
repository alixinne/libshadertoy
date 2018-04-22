#include <fstream>
#include <map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/basic_input.hpp"

#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/toy_buffer.hpp"
#include "shadertoy/render_context.hpp"

using namespace std;
namespace fs = boost::filesystem;

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
	context.build_buffer_shader(id(), fs_, source_files_);

	// Attach shader
	program_.attach_shader(fs_);

	// Link the program
	program_.link();

	// Use the program
	program_.use();

	// bind uniform inputs
	bound_inputs_ = context.bound_inputs(program_);
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
	auto &state(static_pointer_cast<shader_inputs_t::bound_inputs>(bound_inputs_[0])->state);
	auto &resolutions(state.get<iChannelResolution>());

	// Setup the texture targets
	for (size_t i = 0; i < inputs_.size(); ++i)
	{
		auto &input(inputs_[i]);

		if (input)
		{
			auto &texture(*input->use());

			input->sampler().bind(i + 1);
			texture.bind_unit(i + 1);

			texture.get_parameter(0, GL_TEXTURE_WIDTH, &resolutions[i][0]);
			texture.get_parameter(0, GL_TEXTURE_HEIGHT, &resolutions[i][1]);
			resolutions[i][2] = 1.0f;
		}
		else
		{
			gl_call(glBindTextureUnit, i + 1, 0);
			gl_call(glBindSampler, i + 1, 0);

			resolutions[i] = glm::vec3(0.f);
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


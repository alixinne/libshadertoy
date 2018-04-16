#include <fstream>
#include <map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl.hpp"

#include "shadertoy/buffer_config.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/uniform_state.hpp"
#include "shadertoy/toy_buffer.hpp"
#include "shadertoy/texture_engine.hpp"
#include "shadertoy/render_context.hpp"

using namespace std;
namespace fs = boost::filesystem;

using namespace shadertoy;
using shadertoy::gl::gl_call;

toy_buffer::toy_buffer(const std::string &id)
	: buffer_base(id),
	  fs_(GL_FRAGMENT_SHADER),
	  bound_inputs_()
{
}

void toy_buffer::init_contents(render_context &context, int width, int height)
{
	// Attach the vertex shader for the screen quad
	program_.attach_shader(context.screen_quad_vertex_shader());

	// Load the fragment shader for this buffer
    context.build_buffer_shader(id(), fs_);

	// Attach shader
	program_.attach_shader(fs_);

	// Link the program
	program_.link();

	// Use the program
	program_.use();

	// bind uniform inputs
	bound_inputs_ = context.bound_inputs(program_);
}

void toy_buffer::render_contents(render_context &context)
{
	auto &config(std::find_if(context.config().buffer_configs.begin(),
		context.config().buffer_configs.end(),
		[this](const auto &pair) { return pair.first == this->id(); })->second);

	// Prepare the render target
	context.clear(0.f);

	// Setup program and its uniforms
	program_.use();

	// Override values in bound inputs 0 (ShaderToy inputs)
	auto &resolutions(static_pointer_cast<shader_inputs_t::bound_inputs>(bound_inputs_[0])
		->state.get<iChannelResolution>());

	// Setup the texture targets
	for (int i = 0; i < 4; ++i)
	{
		gl_call(glActiveTexture, GL_TEXTURE0 + i + 1);
		// Following have side effects, ensure it runs after we selected the new
		// texture unit
		auto &texture = context.tex_engine()
                .input_texture(config.inputs[i]);
		texture.bind(GL_TEXTURE_2D);

		texture.get_parameter(0, GL_TEXTURE_WIDTH, &resolutions[i][0]);
		texture.get_parameter(0, GL_TEXTURE_HEIGHT, &resolutions[i][1]);
		resolutions[i][2] = 1.0f;
	}

	// Try to set iTimeDelta
	GLint available = 0;
	time_delta_query().get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
	if (available)
	{
		// Result available, set uniform value
		GLuint64 timeDelta;
		time_delta_query().get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
		static_pointer_cast<shader_inputs_t::bound_inputs>(bound_inputs_[0])
			->state.get<iTimeDelta>() = timeDelta / 1e9;
	}

	// Set all uniforms
	for (auto &inputs : bound_inputs_)
        inputs->apply();

	// Render the program
    context.render_screen_quad(time_delta_query());
}


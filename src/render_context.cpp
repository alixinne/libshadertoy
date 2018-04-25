#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "resources.h"
#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/toy_buffer.hpp"
#include "shadertoy/shader_compiler.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/compiler/template_part.hpp"
#include "shadertoy/compiler/define_part.hpp"
#include "shadertoy/compiler/input_part.hpp"

#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/swap_chain.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;
using shadertoy::gl::gl_call;

void render_context::load_buffer_sources(compiler::shader_template &buffer_template) const
{
}

void render_context::bind_inputs(std::vector<std::shared_ptr<bound_inputs_base>> &inputs,
                                 const gl::program &program) const
{
}

render_context::render_context()
	: screen_vs_(GL_VERTEX_SHADER),
	screen_fs_(GL_FRAGMENT_SHADER),
	buffer_template_{
		compiler::template_part("glsl:header", std::string(glsl_header_frag, glsl_header_frag + glsl_header_frag_size)),
		compiler::define_part("glsl:defines"),
		compiler::template_part("shadertoy:header", std::string(shadertoy_header_frag, shadertoy_header_frag + shadertoy_header_frag_size)),
		compiler::template_part("shadertoy:uniforms", state_.definitions_string()),
		compiler::template_part("buffer:inputs"),
		compiler::template_part("buffer:sources"),
		compiler::template_part("shadertoy:footer", std::string(shadertoy_footer_frag, shadertoy_footer_frag + shadertoy_footer_frag_size))
	},
	error_input_(std::make_shared<inputs::error_input>())
{
	// Prepare screen quad geometry
	GLfloat coords[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	// Setup coords
	screen_quad_corners_.data(sizeof(coords), static_cast<const GLvoid *>(&coords[0]), GL_STATIC_DRAW);

	// Setup indices
	scren_quad_indices_.data(sizeof(indices), static_cast<const GLvoid *>(&indices[0]), GL_STATIC_DRAW);

	// Compile screen quad vertex shader
	screen_vs_.source(std::string(screenQuad_vsh, screenQuad_vsh + screenQuad_vsh_size));
	screen_vs_.compile();

	// Compile screen quad fragment shader
	screen_fs_.source(std::string(screenQuad_fsh, screenQuad_fsh + screenQuad_fsh_size));
	screen_fs_.compile();

	// Prepare screen quad program
	screen_prog_.attach_shader(screen_vs_);
	screen_prog_.attach_shader(screen_fs_);

	// Compile screen program
	screen_prog_.link();

	// Setup screen textures
	screen_prog_.use();
	screen_prog_.get_uniform_location("screenTexture").set_value(0);

	// Setup position and texCoord attributes for shaders
	screen_quad_corners_.bind(GL_ARRAY_BUFFER);
	scren_quad_indices_.bind(GL_ELEMENT_ARRAY_BUFFER);

	std::vector<gl::program *> programs{ &screen_prog_ };

	for (auto it = programs.begin(); it != programs.end(); ++it)
	{
		// bind input "position" to vertex locations (3 floats)
		auto position = (*it)->get_attrib_location("position");
		position.vertex_pointer(3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
		position.enable_vertex_array();

		// bind input "texCoord" to vertex texture coordinates (2 floats)
		auto texCoord = (*it)->get_attrib_location("texCoord");
		texCoord.vertex_pointer(2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		texCoord.enable_vertex_array();
	}

	// Set uniform texture units
	state_.get<iChannel0>() = 0;
	state_.get<iChannel1>() = 1;
	state_.get<iChannel2>() = 2;
	state_.get<iChannel3>() = 3;

	state_.get<iChannelTime>() = { 0.f, 0.f, 0.f, 0.f };
	state_.get<iSampleRate>() = 48000.f;
}

void render_context::init(swap_chain &chain) const
{
	chain.init(*this);
	chain.allocate_textures(*this);
}

void render_context::allocate_textures(swap_chain &chain) const
{
	chain.allocate_textures(*this);
}

std::shared_ptr<members::basic_member> render_context::render(swap_chain &chain) const
{
	return chain.render(*this);
}

void render_context::build_buffer_shader(const buffers::toy_buffer &buffer, gl::shader &fs) const
{
	// Load all source parts
	auto fs_template(buffer_template_.specify({
		compiler::input_part("buffer:inputs", buffer.inputs()),
		compiler::template_part::from_files("buffer:sources", buffer.source_files()),
	}));

	// Load callback sources
    load_buffer_sources(fs_template);

	// Load sources into fragment shader and compile
	shader_compiler::compile(fs, fs_template.sources());
}

std::vector<std::shared_ptr<bound_inputs_base>> render_context::bound_inputs(gl::program &program) const
{
	std::vector<std::shared_ptr<bound_inputs_base>> result;

	// External inputs
	bind_inputs(result, program);

	// Default inputs
	result.insert(result.begin(), state_.bind_inputs(program));

	return result;
}

void render_context::render_screen_quad() const
{
	screen_quad_corners_.bind(GL_ARRAY_BUFFER);
	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);
}

void render_context::render_screen_quad(const gl::query &timerQuery) const
{
	screen_quad_corners_.bind(GL_ARRAY_BUFFER);

	timerQuery.begin(GL_TIME_ELAPSED);

	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);

	timerQuery.end(GL_TIME_ELAPSED);
}

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "resources.hpp"
#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/program_buffer.hpp"
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

render_context::render_context()
	: state_(),
	buffer_template_(),
	error_input_(std::make_shared<inputs::error_input>())
{
	buffer_template_.emplace(GL_VERTEX_SHADER, compiler::shader_template(
		compiler::template_part("shadertoy:screenquad", std::string(screenQuad_vsh, screenQuad_vsh + screenQuad_vsh_size))
	));

	buffer_template_.emplace(GL_FRAGMENT_SHADER, compiler::shader_template(
		compiler::template_part("glsl:header", std::string(glsl_header_frag, glsl_header_frag + glsl_header_frag_size)),
		compiler::define_part("glsl:defines"),
		compiler::template_part("shadertoy:header", std::string(shadertoy_header_frag, shadertoy_header_frag + shadertoy_header_frag_size)),
		compiler::template_part("shadertoy:uniforms", state_.definitions_string()),
		compiler::template_part("buffer:inputs"),
		compiler::template_part("buffer:sources"),
		compiler::template_part("shadertoy:footer", std::string(shadertoy_footer_frag, shadertoy_footer_frag + shadertoy_footer_frag_size))
	));

	// Register state
	buffer_template_.shader_inputs().push_back(&state_);

	// Add LIBSHADERTOY definition
	static_cast<compiler::define_part*>(buffer_template_[GL_FRAGMENT_SHADER].find("glsl:defines").get())
		->definitions()
		->definitions()
		.insert(std::make_pair<std::string, std::string>("LIBSHADERTOY", "1"));

	log::shadertoy()->trace("Compiling screen programs for context {}", (void*)this);

	// Compile screen quad vertex shader
	buffer_template_.compile(GL_VERTEX_SHADER);

	// Compile screen program
	std::map<GLenum, compiler::shader_template> overrides;
	overrides.emplace(GL_FRAGMENT_SHADER, compiler::shader_template(
		compiler::template_part("shadertoy:screenquad", std::string(screenQuad_fsh, screenQuad_fsh + screenQuad_fsh_size)
	)));
	screen_prog_ = buffer_template_.compile(overrides);

	// Set uniform texture units
	state_.get<iChannel0>() = 0;
	state_.get<iChannel1>() = 1;
	state_.get<iChannel2>() = 2;
	state_.get<iChannel3>() = 3;

	state_.get<iChannelTime>() = { 0.f, 0.f, 0.f, 0.f };
	state_.get<iSampleRate>() = 48000.f;

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

	// Bind VAO
	screen_quad_array_.bind();

	// Setup coords
	screen_quad_corners_.data(sizeof(coords), static_cast<const GLvoid *>(&coords[0]), GL_STATIC_DRAW);

	// Setup indices
	screen_quad_indices_.data(sizeof(indices), static_cast<const GLvoid *>(&indices[0]), GL_STATIC_DRAW);

	// Setup position and texCoord attributes for shaders
	screen_quad_corners_.bind(GL_ARRAY_BUFFER);
	screen_quad_indices_.bind(GL_ELEMENT_ARRAY_BUFFER);

	// bind input "position" to vertex locations (3 floats)
	gl::attrib_location position(0);
	position.vertex_pointer(3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
	position.enable_vertex_array();

	// bind input "texCoord" to vertex texture coordinates (2 floats)
	gl::attrib_location texCoord(1);
	texCoord.vertex_pointer(2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	texCoord.enable_vertex_array();

	// Unbind
	screen_quad_array_.unbind();
	screen_quad_indices_.unbind(GL_ELEMENT_ARRAY_BUFFER);
	screen_quad_corners_.unbind(GL_ARRAY_BUFFER);
}

void render_context::init(swap_chain &chain) const
{
	log::shadertoy()->trace("Initializing chain {}", (void*)&chain);
	chain.init(*this);

	allocate_textures(chain);
}

void render_context::allocate_textures(swap_chain &chain) const
{
	log::shadertoy()->trace("Allocating chain {}", (void*)&chain);
	chain.allocate_textures(*this);
}

std::shared_ptr<members::basic_member> render_context::render(swap_chain &chain) const
{
	return chain.render(*this);
}

void render_context::render_screen_quad() const
{
	// Bind VAO
	auto sqa_bind(gl::get_bind_guard(screen_quad_array_));

	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);
}

void render_context::render_screen_quad(const gl::query &timerQuery) const
{
	// Bind VAO
	auto sqa_bind(gl::get_bind_guard(screen_quad_array_));

	timerQuery.begin(GL_TIME_ELAPSED);

	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);

	timerQuery.end(GL_TIME_ELAPSED);
}

// vim: cino=

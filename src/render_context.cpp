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

#include "shadertoy/geometry/screen_quad.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;
using shadertoy::gl::gl_call;

render_context::render_context()
	: state_(),
	buffer_template_(),
	error_input_(std::make_shared<inputs::error_input>())
{
	auto preprocessor_defines(std::make_shared<compiler::preprocessor_defines>());

	// Add LIBSHADERTOY definition
	preprocessor_defines->definitions().emplace("LIBSHADERTOY", "1");

	buffer_template_.shader_defines().emplace("glsl", preprocessor_defines);
	buffer_template_.shader_inputs().emplace("shadertoy", &state_);

	buffer_template_.emplace(GL_VERTEX_SHADER,
		compiler::shader_template::parse(std::string(screenQuad_vsh, screenQuad_vsh + screenQuad_vsh_size), "libshadertoy/shaders/screenQuad.vsh")
	);

	buffer_template_.emplace(GL_FRAGMENT_SHADER,
		compiler::shader_template::parse(std::string(shadertoy_frag_glsl, shadertoy_frag_glsl + shadertoy_frag_glsl_size), "libshadertoy/shaders/shadertoy_frag.glsl")
	);

	// Compile screen quad vertex shader
	buffer_template_.compile(GL_VERTEX_SHADER);

	// Set uniform texture units
	state_.get<iChannel0>() = 0;
	state_.get<iChannel1>() = 1;
	state_.get<iChannel2>() = 2;
	state_.get<iChannel3>() = 3;

	state_.get<iChannelTime>() = { 0.f, 0.f, 0.f, 0.f };
	state_.get<iSampleRate>() = 48000.f;
}

const gl::program &render_context::screen_prog() const
{
	if (!screen_prog_)
	{
		log::shadertoy()->trace("Compiling screen programs for context {}", (void*)this);

		// Compile screen program
		std::map<GLenum, compiler::shader_template> overrides;
		overrides.emplace(GL_FRAGMENT_SHADER, compiler::shader_template(
			compiler::template_part("shadertoy:screenquad", std::string(screenQuad_fsh, screenQuad_fsh + screenQuad_fsh_size)
		)));

		screen_prog_ = std::make_unique<gl::program>(buffer_template_.compile(overrides));
	}

	return *screen_prog_;
}

const geometry::screen_quad &render_context::screen_quad() const
{
	if (!screen_quad_)
	{
		log::shadertoy()->trace("Initializing screen quad geometry for {}", (void*)this);
		screen_quad_ = std::make_unique<geometry::screen_quad>();
	}

	return *screen_quad_;
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

// vim: cino=

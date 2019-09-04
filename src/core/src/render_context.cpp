#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/utils/log.hpp"

#include "resources.hpp"
#include "shadertoy/buffers/program_buffer.hpp"
#include "shadertoy/render_context.hpp"
#include "shadertoy/shader_compiler.hpp"

#include "shadertoy/compiler/define_part.hpp"
#include "shadertoy/compiler/input_part.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/swap_chain.hpp"

#include "shadertoy/geometry/screen_quad.hpp"

#include "shadertoy/sources/string_source.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;

render_context::render_context()
: buffer_template_(std::make_shared<compiler::program_template>()),
  error_input_(std::make_shared<inputs::error_input>())
{
	auto preprocessor_defines(std::make_shared<compiler::preprocessor_defines>());

	// Add LIBSHADERTOY definition
	preprocessor_defines->definitions().emplace("LIBSHADERTOY", "1");

	buffer_template_->shader_defines().emplace("glsl", preprocessor_defines);

#if SHADERTOY_HAS_GLSL_440
	buffer_template_->emplace(GL_VERTEX_SHADER, compiler::shader_template::parse(
												std::string(std::addressof(screen_quad_vert_glsl[0]), screen_quad_vert_glsl_size),
												"libshadertoy/shaders/screen_quad.vert.glsl"));

	buffer_template_->emplace(GL_FRAGMENT_SHADER,
							  compiler::shader_template::parse(
							  std::string(std::addressof(shadertoy_frag_glsl[0]), shadertoy_frag_glsl_size),
							  "libshadertoy/shaders/shadertoy.frag.glsl"));

	buffer_template_->emplace(GL_COMPUTE_SHADER,
							  compiler::shader_template::parse(
							  std::string(std::addressof(shadertoy_comp_glsl[0]), shadertoy_comp_glsl_size),
							  "libshadertoy/shaders/shadertoy.comp.glsl"));
#else
	buffer_template_->emplace(GL_VERTEX_SHADER, compiler::shader_template::parse(
												std::string(std::addressof(screen_quad_vert_es3_glsl[0]),
															screen_quad_vert_es3_glsl_size),
												"libshadertoy/shaders/screen_quad.vert.es3.glsl"));

	buffer_template_->emplace(GL_FRAGMENT_SHADER,
							  compiler::shader_template::parse(
							  std::string(std::addressof(shadertoy_frag_es3_glsl[0]), shadertoy_frag_es3_glsl_size),
							  "libshadertoy/shaders/shadertoy.frag.es3.glsl"));

#endif

	// Compile screen quad vertex shader
	buffer_template_->compile(GL_VERTEX_SHADER);
}

const backends::gx::program &render_context::screen_prog() const
{
	if (!screen_prog_)
	{
		log::shadertoy()->trace("Compiling screen programs for context {}", static_cast<const void *>(this));

#if SHADERTOY_HAS_GLSL_440
		sources::string_source screen_prog_sources(
		{ { GL_VERTEX_SHADER,
			{ "libshadertoy/shaders/screen_quad.vert.glsl",
			  std::string(std::addressof(screen_quad_vert_glsl[0]), screen_quad_vert_glsl_size) } },
		  { GL_FRAGMENT_SHADER,
			{ "libshadertoy/shaders/screen_quad.frag.glsl",
			  std::string(std::addressof(screen_quad_frag_glsl[0]), screen_quad_frag_glsl_size) } } });
#else
		sources::string_source screen_prog_sources(
		{ { GL_VERTEX_SHADER,
			{ "libshadertoy/shaders/screen_quad.vert.es3.glsl",
			  std::string(std::addressof(screen_quad_vert_es3_glsl[0]), screen_quad_vert_es3_glsl_size) } },
		  { GL_FRAGMENT_SHADER,
			{ "libshadertoy/shaders/screen_quad.frag.es3.glsl",
			  std::string(std::addressof(screen_quad_frag_es3_glsl[0]), screen_quad_frag_es3_glsl_size) } } });
#endif

		// Compile screen program
		screen_prog_ = screen_prog_sources.compile_program({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER });
	}

	return *screen_prog_;
}

const geometry::screen_quad &render_context::screen_quad() const
{
	if (!screen_quad_)
	{
		log::shadertoy()->trace("Initializing screen quad geometry for {}", static_cast<const void *>(this));
		screen_quad_ = std::make_unique<geometry::screen_quad>();
	}

	return *screen_quad_;
}

void render_context::init(swap_chain &chain) const
{
	log::shadertoy()->trace("Initializing chain {}", static_cast<const void *>(&chain));
	chain.init(*this);

	allocate_textures(chain);
}

void render_context::allocate_textures(swap_chain &chain) const
{
	log::shadertoy()->trace("Allocating chain {}", static_cast<const void *>(&chain));
	chain.allocate_textures(*this);
}

std::shared_ptr<members::basic_member> render_context::render(swap_chain &chain) const
{
	return chain.render(*this);
}

// vim: cino=

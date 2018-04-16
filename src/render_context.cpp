#include <fstream>
#include <map>
#include <memory>
#include <sstream>

#include <boost/filesystem.hpp>

#include <boost/variant.hpp>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "resources.h"
#include "shadertoy/context_config.hpp"
#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffer_config.hpp"
#include "shadertoy/toy_buffer.hpp"
#include "shadertoy/texture_engine.hpp"
#include "shadertoy/shader_compiler.hpp"
#include "shadertoy/render_context.hpp"

namespace fs = boost::filesystem;
using namespace std;
using namespace shadertoy;
using namespace shadertoy::utils;
using shadertoy::gl::gl_call;

void render_context::check_render_size(size_t width, size_t height)
{
	if (width == 0)
		throw shadertoy_error("The rendering width must be greater than 0");

	if (height == 0)
		throw shadertoy_error("The rendering height must be greater than 0");
}

unique_ptr<texture_engine> render_context::build_texture_engine()
{
	auto engine = make_unique<texture_engine>(config_);

	engine->register_handler("buffer", input_handler([this]
		(const input_config &inputConfig,
		 bool &skipTextureOptions,
		 bool &skipCache,
		 bool &framebufferSized)
	{
		skipTextureOptions = true;
		skipCache = true;
		// No need to reallocate buffer textures, this is handled by the buffer
		// itself
		framebufferSized = false;

		auto &bufferConfigs = config_.buffer_configs;
		auto bufferIt = std::find_if(bufferConfigs.begin(), bufferConfigs.end(),
			[&inputConfig](const auto &pair) { return pair.first == inputConfig.source; });

		if (bufferIt == bufferConfigs.end())
		{
			log::shadertoy()->warn("Buffer '{}' not found for input {}", inputConfig.source, inputConfig.id);
			return shared_ptr<gl::texture>();
		}
		else
		{
			if (frame_count_ == 0)
			{
				log::shadertoy()->info("Binding '{}' to input {}", inputConfig.source, inputConfig.id);
			}

			auto texture = buffers_[inputConfig.source]->source_texture();

			GLint minFilter = inputConfig.min_filter,
				  magFilter = inputConfig.mag_filter;

			texture->parameter(GL_TEXTURE_MAG_FILTER, magFilter);
			texture->parameter(GL_TEXTURE_MIN_FILTER, minFilter);
			texture->parameter(GL_TEXTURE_WRAP_S, inputConfig.wrap);
			texture->parameter(GL_TEXTURE_WRAP_T, inputConfig.wrap);

			if (minFilter > GL_LINEAR)
				texture->generate_mipmap();

			return texture;
		}
	}));

	return engine;
}

void render_context::pre_init_buffers()
{
}

void render_context::post_init_buffers()
{
}

void render_context::load_buffer_sources(vector<pair<string, string>> &sources)
{
}

void render_context::post_render_buffer(const string &name,
                                        shared_ptr<buffer_base> &buffer)
{
}

void render_context::bind_inputs(vector<shared_ptr<bound_inputs_base>> &inputs,
                                 gl::program &program)
{
}

render_context::render_context(context_config &config)
	: config_(config),
	screen_vs_(GL_VERTEX_SHADER),
	screen_fs_(GL_FRAGMENT_SHADER),
	frame_count_(0)
{
	tex_engine_ = build_texture_engine();

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
	screen_vs_.source(string(screenQuad_vsh, screenQuad_vsh + screenQuad_vsh_size));
	screen_vs_.compile();

	// Compile screen quad fragment shader
	screen_fs_.source(string(screenQuad_fsh, screenQuad_fsh + screenQuad_fsh_size));
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

	vector<gl::program *> programs{ &screen_prog_ };

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
}

void render_context::init()
{
	check_render_size(config_.width, config_.height);

	// Initialize constant uniforms
	state_.get<iResolution>() = glm::vec3(config_.width, config_.height, 1.0f);
	// Note that this will be overriden once query measurements are available
	state_.get<iTimeDelta>() = 1.0f / (float)config_.target_framerate;
	state_.get<iFrameRate>() = (float)config_.target_framerate;

	state_.get<iChannel0>() = 1;
	state_.get<iChannel1>() = 2;
	state_.get<iChannel2>() = 3;
	state_.get<iChannel3>() = 4;

	state_.get<iChannelTime>() = { 0.f, 0.f, 0.f, 0.f };
	state_.get<iSampleRate>() = 48000.f;

	// Initialize the texture engine
	tex_engine_->init();

	// Initialize buffers
	init_buffers();
}

void render_context::init_buffers()
{
	// Invoke callback
	pre_init_buffers();

	// Prepare the define wrapper
	ostringstream oss;
	for (auto define : config_.preprocessor_defines)
	{
		oss << "#define " << define.first;
		if (!define.second.empty())
		{
			oss << " " << define.second;
		}
		oss << endl;
	}
	define_wrapper_ = oss.str();

	// Initialize program buffer
	auto bufferConfigs = config_.buffer_configs;
	for (auto it = bufferConfigs.begin(); it != bufferConfigs.end(); ++it)
	{
		auto buf = make_shared<toy_buffer>(it->first);
		buf->init(*this, config_.width, config_.height);
		buffers_.insert(make_pair(it->first, buf));
	}

	last_texture_ = weak_ptr<gl::texture>();

	// Invoke callback
	post_init_buffers();
}

void render_context::allocate_textures()
{
	check_render_size(config_.width, config_.height);

	// Drop the reference to screen_quad_texture_, it will be recreated if needed
	screen_quad_texture_ = shared_ptr<gl::texture>();

	// Reallocate buffer textures
	for (auto &pair : buffers_)
		pair.second->allocate_textures(config_.width, config_.height);

	// Reallocate inputs
	tex_engine_->clear(true);

	// Update the iResolution uniform, as this method can be called after a
	// framebuffer size change
	state_.get<iResolution>() = glm::vec3(config_.width, config_.height, 1.0f);
}

void render_context::clear_state()
{
	// Clear previous input textures
	tex_engine_->clear();
	// Clear previous buffers
	buffers_.clear();
	// Clear the source cache
	source_cache_.clear();
}

void render_context::render()
{
	for (auto buffer_spec : config_.buffer_configs)
	{
		auto &buffer(buffers_[buffer_spec.first]);

		buffer->render(*this);
		last_texture_ = buffer->source_texture();

		post_render_buffer(buffer_spec.first, buffer);
	}

	frame_count_++;
}

void render_context::read_write_current_frame(GLuint &texIn, GLuint &texOut)
{
	if (auto currentTex = last_texture_.lock())
	{
		// Allocate the target screen quad texture as it is requested
		if (!screen_quad_texture_)
		{
			// Create texture object
			screen_quad_texture_ = make_shared<gl::texture>(GL_TEXTURE_2D);

			// Setup screen_quad_texture_
			screen_quad_texture_->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			screen_quad_texture_->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			screen_quad_texture_->parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
			screen_quad_texture_->parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
			screen_quad_texture_->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F,
				config_.width, config_.height, 0, GL_BGRA, GL_FLOAT, nullptr);
		}

		texIn = *currentTex;
		texOut = *screen_quad_texture_;

		last_texture_ = screen_quad_texture_;
	}
	else
	{
		throw runtime_error(string(__func__) + ": last_texture_ pointer has expired!");
	}
}

void render_context::read_current_frame(GLuint &texIn)
{
	if (auto currentTex = last_texture_.lock())
	{
		texIn = *currentTex;
	}
	else
	{
		throw runtime_error(string(__func__) + ": last_texture_ pointer has expired!");
	}
}

void render_context::build_buffer_shader(const string &id,
                                         gl::shader &fs)
{
	auto &bufferConfig(std::find_if(config_.buffer_configs.begin(),
		config_.buffer_configs.end(),
		[&id](const auto &pair) { return pair.first == id; })->second);

	// Load all source parts
	shader_compiler compiler;
	auto &sources(compiler.sources());

	// Load callback sources
    load_buffer_sources(sources);

	// Add define wrapper
	sources.insert(sources.begin(),
		make_pair(string("generated:define-wrapper"), define_wrapper_));

	// Add sources
	for (auto &shaderFile : bufferConfig.shader_files)
	{
		sources.push_back(make_pair(shaderFile.string(), load_shader_source(shaderFile)));
	}

	// Add default wrapper around code
	sources.insert(sources.begin(), make_pair(
		string("internal:wrapper-header"),
		string(wrapper_header_fsh, wrapper_header_fsh + wrapper_header_fsh_size)));

	// Add source from uniform declarations
	sources.insert(sources.begin() + 1, make_pair(
		string("generated:shader-inputs"),
		state_.definitions_string()));

	// Add footer
	sources.push_back(make_pair(
		string("internal:wrapper-footer"),
		string(wrapper_footer_fsh, wrapper_footer_fsh + wrapper_footer_fsh_size)));

	// Load sources into fragment shader and compile
	compiler.compile(fs);
}

const GLchar *render_context::load_shader_source(const fs::path &path) throw(runtime_error)
{
	fs::path shaderPath(fs::canonical(path));
	ifstream src(shaderPath.string().c_str());
	string loadedSource;
	loadedSource.assign(istreambuf_iterator<char>(src), istreambuf_iterator<char>());
	if (loadedSource.back() != '\n')
	{
		loadedSource += "\n";
	}
	source_cache_.insert(make_pair(shaderPath.string(), loadedSource));

	log::shadertoy()->info("Loaded {}", shaderPath);

	return source_cache_.find(shaderPath.string())->second.c_str();
}

const GLchar *render_context::define_wrapper() const
{
	return define_wrapper_.c_str();
}

vector<shared_ptr<bound_inputs_base>> render_context::bound_inputs(gl::program &program)
{
	vector<shared_ptr<bound_inputs_base>> result;

	// External inputs
	bind_inputs(result, program);

	// Default inputs
	result.insert(result.begin(), state_.bind_inputs(program));

	return result;
}

void render_context::clear(float level)
{
	gl_call(glViewport, 0, 0, config_.width, config_.height);
	gl_call(glClearColor, level, level, level, level);
	gl_call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void render_context::render_screen_quad()
{
	screen_quad_corners_.bind(GL_ARRAY_BUFFER);
	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);
}

void render_context::render_screen_quad(gl::query &timerQuery)
{
	screen_quad_corners_.bind(GL_ARRAY_BUFFER);

	timerQuery.begin(GL_TIME_ELAPSED);

	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);

	timerQuery.end(GL_TIME_ELAPSED);
}

void render_context::bind_result()
{
	// Prepare prog and texture
	screen_prog_.use();

	gl_call(glActiveTexture, GL_TEXTURE0);
	if (auto ptr = last_texture_.lock())
	{
		ptr->bind(GL_TEXTURE_2D);
		ptr->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		ptr->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

gl::shader &render_context::screen_quad_vertex_shader()
{
	return screen_vs_;
}

shared_ptr<buffer_base> render_context::buffer(const string &name)
{
	if (name.empty())
	{
		if (buffers_.empty())
		{
			return shared_ptr<buffer_base>();
		}

		return buffers_.rbegin()->second;
	}
	else
	{
		auto it = buffers_.find(name);
		if (it == buffers_.end())
		{
			return shared_ptr<buffer_base>();
		}

		return it->second;
	}
}

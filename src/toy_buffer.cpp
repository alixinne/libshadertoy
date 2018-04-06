#include <fstream>
#include <map>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
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

toy_buffer::toy_buffer(render_context &context, const std::string &id)
    : context_(context), id_(id), fs_(GL_FRAGMENT_SHADER), bound_inputs_(),
      time_delta_query_(GL_TIME_ELAPSED) {}

void toy_buffer::init(int width, int height) {
  auto &config(
      std::find_if(context_.config().buffer_configs.begin(),
                   context_.config().buffer_configs.end(),
                   [this](const auto &pair) { return pair.first == id_; })
          ->second);

  // Attach the vertex shader for the screen quad
  program_.attach_shader(context_.screen_quad_vertex_shader());

  // Load the fragment shader for this buffer
  context_.build_buffer_shader(id_, fs_);

  // Attach shader
  program_.attach_shader(fs_);

  // Link the program
  program_.link();

  // Use the program
  program_.use();

  // Dump the program if requested
  if (context_.config().dump_shaders) {
    // Allocate buffer
    GLint len, actLen;
    program_.get(GL_PROGRAM_BINARY_LENGTH, &len);

    std::vector<char> progBinary(len);
    // Get binary
    GLenum format;
    program_.get_binary(progBinary.size(), &actLen, &format, progBinary.data());

    // Store in file
    fs::path name(config.shader_files.front());
    name.replace_extension(".dump");
    BOOST_LOG_TRIVIAL(info) << "dumping program binary to " << name;

    ofstream ofs(name.string().c_str(), std::ofstream::binary);

    if (!ofs.is_open()) {
      std::stringstream ss;
      ss << "could not open output file " << name.string();
      throw std::runtime_error(ss.str());
    }

    ofs.write(progBinary.data(), actLen);
    ofs.close();
  }

  // bind uniform inputs
  bound_inputs_ = context_.bound_inputs(program_);

  // Allocate render textures
  allocate_textures(width, height);
}

void toy_buffer::allocate_textures(int width, int height)
{
	// Initialize buffer textures
    init_render_texture(source_tex_, width, height);
    init_render_texture(target_tex_, width, height);

	// Setup render buffers
	target_tex_->bind(GL_TEXTURE_2D);
	target_rbo_.bind(GL_RENDERBUFFER);
	target_rbo_.storage(GL_DEPTH_COMPONENT, width, height);
}

void toy_buffer::render() {
  auto &config(
      std::find_if(context_.config().buffer_configs.begin(),
                   context_.config().buffer_configs.end(),
                   [this](const auto &pair) { return pair.first == id_; })
          ->second);

  // Update renderbuffer to use the correct target texture
  target_tex_->bind(GL_TEXTURE_2D);
  target_rbo_.bind(GL_RENDERBUFFER);
  target_fbo_.bind(GL_DRAW_FRAMEBUFFER);

  target_fbo_.texture(GL_COLOR_ATTACHMENT0, *target_tex_, 0);

  // Prepare the render target
  context_.clear(0.f);

  // Setup program and its uniforms
  program_.use();

  // Override values in bound inputs 0 (ShaderToy inputs)
  auto &resolutions(
      static_pointer_cast<shader_inputs_t::bound_inputs>(bound_inputs_[0])
          ->state.get<iChannelResolution>());

  // Setup the texture targets
  for (int i = 0; i < 4; ++i) {
    gl_call(glActiveTexture, GL_TEXTURE0 + i + 1);
    // Following have side effects, ensure it runs after we selected the new
    // texture unit
    auto &texture = context_.tex_engine().input_texture(config.inputs[i]);
    texture.bind(GL_TEXTURE_2D);

    texture.get_parameter(0, GL_TEXTURE_WIDTH, &resolutions[i][0]);
    texture.get_parameter(0, GL_TEXTURE_HEIGHT, &resolutions[i][1]);
    resolutions[i][2] = 1.0f;
  }

  // Try to set iTimeDelta
  GLint available = 0;
  time_delta_query_.get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
  if (available) {
    // Result available, set uniform value
    GLuint64 timeDelta;
    time_delta_query_.get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
    static_pointer_cast<shader_inputs_t::bound_inputs>(bound_inputs_[0])
        ->state.get<iTimeDelta>() = timeDelta / 1e9;
  }

  // Set all uniforms
  for (auto &inputs : bound_inputs_)
    inputs->apply();

  // Render the program
  context_.render_screen_quad(time_delta_query_);

  // Swap texture object pointers
  swap(source_tex_, target_tex_);
}

unsigned long long toy_buffer::elapsed_time()
{
	GLint available = 0;

	// Wait for result to be available
	while (!available)
	{
		time_delta_query_.get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
	}

	// Fetch result
	GLuint64 result;
	time_delta_query_.get_object_ui64v(GL_QUERY_RESULT, &result);

	return result;
}

void toy_buffer::init_render_texture(shared_ptr<gl::texture> &texptr, int width, int height)
{
	// Only create a texture object if it is necessary
	if (!texptr)
		texptr = make_shared<gl::texture>(GL_TEXTURE_2D);

	// Allocate texture storage according to width/height
	texptr->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	texptr->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texptr->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texptr->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	texptr->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	float black[4] = {0.f};
	texptr->clear_tex_image(0, GL_BGRA, GL_FLOAT, black);
}

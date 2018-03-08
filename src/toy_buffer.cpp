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
    : context(context), id(id), fs(GL_FRAGMENT_SHADER), boundInputs(),
      timeDeltaQuery(GL_TIME_ELAPSED) {}

void toy_buffer::Initialize(int width, int height) {
  auto &config(context.GetConfig().bufferConfigs[id]);

  // Attach the vertex shader for the screen quad
  program.attach_shader(context.GetScreenQuadVertexShader());

  // Load the fragment shader for this buffer
  context.BuildBufferShader(id, fs);

  // Attach shader
  program.attach_shader(fs);

  // Link the program
  program.link();

  // Use the program
  program.use();

  // Dump the program if requested
  if (context.GetConfig().dumpShaders) {
    GLenum err;
    // Get GL program id
    GLuint progId = GLuint(program);
    // Allocate buffer
    GLint len, actLen;
    gl_call(glGetProgramiv, progId, GL_PROGRAM_BINARY_LENGTH, &len);

    char *progBinary = new char[len];
    // Get binary
    GLenum format;
    glGetProgramBinary(progId, len, &actLen, &format, progBinary);
    if ((err = glGetError()) != GL_NO_ERROR) {
      delete[] progBinary;

      std::stringstream ss;
      ss << "could not get program binary: " << err;
      throw std::runtime_error(ss.str());
    }

    // Store in file
    fs::path name(config.shaderFiles.front());
    name.replace_extension(".dump");
    BOOST_LOG_TRIVIAL(info) << "dumping program binary to " << name;

    ofstream ofs(name.string().c_str(), std::ofstream::binary);

    if (!ofs.is_open()) {
      delete[] progBinary;

      std::stringstream ss;
      ss << "could not open output file " << name.string();
      throw std::runtime_error(ss.str());
    }

    ofs.write(progBinary, actLen);
    ofs.close();
    delete[] progBinary;
  }

  // bind uniform inputs
  boundInputs = context.GetBoundInputs(program);

  // Allocate render textures
  AllocateTextures(width, height);
}

void toy_buffer::AllocateTextures(int width, int height) {
  // Initialize buffer textures
  InitializeRenderTexture(sourceTex, width, height);
  InitializeRenderTexture(targetTex, width, height);

  // Setup render buffers
  targetTex->bind(GL_TEXTURE_2D);
  targetRbo.bind(GL_RENDERBUFFER);
  targetRbo.storage(GL_DEPTH_COMPONENT, width, height);
}

void toy_buffer::Render() {
  auto &config(context.GetConfig().bufferConfigs[id]);

  // Update renderbuffer to use the correct target texture
  targetTex->bind(GL_TEXTURE_2D);
  targetRbo.bind(GL_RENDERBUFFER);
  targetFbo.bind(GL_DRAW_FRAMEBUFFER);

  targetFbo.texture(GL_COLOR_ATTACHMENT0, *targetTex, 0);

  // Prepare the render target
  context.Clear(0.f);

  // Setup program and its uniforms
  program.use();

  // Override values in bound inputs 0 (ShaderToy inputs)
  auto &resolutions(
      static_pointer_cast<ShaderInputsType::BoundInputs>(boundInputs[0])
          ->State.V<iChannelResolution>());

  // Setup the texture targets
  for (int i = 0; i < 4; ++i) {
    gl_call(glActiveTexture, GL_TEXTURE0 + i + 1);
    // Following have side effects, ensure it runs after we selected the new
    // texture unit
    auto &texture =
        context.GetTextureEngine().GetInputTexture(config.inputConfig[i]);
    texture.bind(GL_TEXTURE_2D);

    gl_call(glGetTexLevelParameterfv, GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
            &resolutions[i][0]);
    gl_call(glGetTexLevelParameterfv, GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT,
            &resolutions[i][1]);
    resolutions[i][2] = 1.0f;
  }

  // Try to set iTimeDelta
  GLint available = 0;
  timeDeltaQuery.get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
  if (available) {
    // Result available, set uniform value
    GLuint64 timeDelta;
    timeDeltaQuery.get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
    static_pointer_cast<ShaderInputsType::BoundInputs>(boundInputs[0])
        ->State.V<iTimeDelta>() = timeDelta / 1e9;
  }

  // Set all uniforms
  for (auto &inputs : boundInputs)
    inputs->Apply();

  // Render the program
  context.RenderScreenQuad(timeDeltaQuery);

  // Swap texture object pointers
  swap(sourceTex, targetTex);
}

unsigned long long toy_buffer::GetElapsedTime() {
  GLint available = 0;

  // Wait for result to be available
  while (!available) {
    timeDeltaQuery.get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
  }

  // Fetch result
  GLuint64 result;
  timeDeltaQuery.get_object_ui64v(GL_QUERY_RESULT, &result);

  return result;
}

void toy_buffer::InitializeRenderTexture(shared_ptr<gl::texture> &texptr,
                                         int width, int height) {
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
  gl_call(glClearTexImage, GLuint(*texptr), 0, GL_BGRA, GL_FLOAT, black);
}

#include <epoxy/gl.h>

#include <GLFW/glfw3.h>

#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>

#include "uniforms.hpp"

namespace fs = boost::filesystem;
using shadertoy::gl::gl_call;

// Create a ShaderInputs type, which references the custom
// uniforms (either static, known at compile time, or dynamic, defined at
// runtime)
typedef shadertoy::ShaderInputs<iDynamicFloats> ExampleInputsType;

// Create a custom render context that uses the extra input state
class ExampleRenderContext : public shadertoy::RenderContext {
  // Declare the variable holding the extra uniform state
  ExampleInputsType extraInputs;

  // Add auto-generated wrapper to the sources of all compiled programs
  void LoadBufferSources(
      std::vector<std::pair<std::string, std::string>> &sources) override {
    sources.push_back(std::make_pair(std::string("internal:example-uniforms"),
                                     extraInputs.GetDefinitions()));
  }

  // After compiling a program, bind the inputs from the extra state into the
  // program
  void
  BindInputs(std::vector<std::shared_ptr<shadertoy::BoundInputsBase>> &inputs,
             shadertoy::gl::Program &program) override {
    inputs.push_back(extraInputs.GetBoundInputs(program));
  }

public:
	// Extra state accessor
  ExampleInputsType &ExtraInputs() { return extraInputs; }

  ExampleRenderContext(shadertoy::ContextConfig &config)
      : shadertoy::RenderContext(config), extraInputs() {
    // Add a custom runtime input
    extraInputs.V<iDynamicFloats>().Add<float>("iCustomTime", 0.0f);
        }
};

void SetFramebufferSize(GLFWwindow *window, int width, int height) {
  // Get the context from the window user pointer
  shadertoy::RenderContext &context = *static_cast<shadertoy::RenderContext *>(
      glfwGetWindowUserPointer(window));

  // Reallocate textures
  context.GetConfig().width = width;
  context.GetConfig().height = height;
  context.AllocateTextures();
}

int main(int argc, char *argv[])
{
	int code = 0;

        shadertoy::ContextConfig contextConfig;
        contextConfig.width = 640;
	contextConfig.height = 480;
        contextConfig.targetFramerate = 60.0;

        shadertoy::BufferConfig imageBuffer;
        imageBuffer.name = "image";
        imageBuffer.shaderFiles.push_back(fs::path("..") /
                                          fs::path("shader.glsl"));

        contextConfig.bufferConfigs.insert(
            make_pair(imageBuffer.name, imageBuffer));

        if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 1;
	}

	// Initialize window
	GLFWwindow *window = glfwCreateWindow(contextConfig.width,
										  contextConfig.height,
										  "libshadertoy example 10-gradient",
										  nullptr,
										  nullptr);

	if (!window)
	{
		std::cerr << "Failed to create glfw window" << std::endl;
		code = 1;
	}
	else
	{
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		{
                  ExampleRenderContext context(contextConfig);
                  std::cout << "Created context based on config" << std::endl;

                  try {
                    // Initialize context
                    context.Initialize();
                    std::cout << "Initialized rendering context" << std::endl;
                  } catch (shadertoy::gl::ShaderCompilationError &sce) {
                    std::cerr << "Failed to compile shader: " << sce.log();
                    code = 2;
                  } catch (shadertoy::ShadertoyError &err) {
                    std::cerr << "Error: " << err.what();
                    code = 2;
			}

			if (code)
				exit(code);

			// Now render for 5s
			int frameCount = 0;
			double t = 0.;

			// Set the resize callback
			glfwSetWindowUserPointer(window, &context);
                        glfwSetFramebufferSizeCallback(window,
                                                       SetFramebufferSize);

                        while (!glfwWindowShouldClose(window))
			{
				// Poll events
				glfwPollEvents();

				// Update uniforms
                                context.GetState().V<shadertoy::iTime>() = t;
                                context.GetState().V<shadertoy::iFrame>() =
                                    frameCount;

                                // Update custom uniform
                                context.ExtraInputs()
                                    .V<iDynamicFloats>()
                                    .Get<float>("iCustomTime") =
                                    (int(t) % 2) == 0 ? 1.0f : 0.0f;

                                // Render to texture
                                context.Render();

                                // Render to screen
				//  Setup framebuffer
				gl_call(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);
				gl_call(glViewport, 0, 0, contextConfig.width, contextConfig.height);

				//  Load texture and program
                                context.BindResult();

                                //  Render result
                                context.RenderScreenQuad();

                                // Buffer swapping
				glfwSwapBuffers(window);

				// Update time and framecount
				t = glfwGetTime();
				frameCount++;

				if (t > 5.)
					glfwSetWindowShouldClose(window, true);
			}
		}

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}

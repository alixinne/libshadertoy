#include <epoxy/gl.h>

#include <GLFW/glfw3.h>

#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>

#include "uniforms.hpp"

namespace fs = boost::filesystem;
using shadertoy::gl::gl_call;

// Create a shader_inputs type, which references the custom
// uniforms (either static, known at compile time, or dynamic, defined at
// runtime)
typedef shadertoy::shader_inputs<
	iDynamicFloats
> example_inputs_t;

// Create a custom render context that uses the extra input state
class example_render_context : public shadertoy::render_context {
	// Declare the variable holding the extra uniform state
	example_inputs_t extra_inputs_;

	// Add auto-generated wrapper to the sources of all compiled programs
	void load_buffer_sources(std::vector<std::pair<std::string, std::string>> &sources) override
	{
		sources.push_back(std::make_pair(
			std::string("internal:example-uniforms"),
			extra_inputs_.definitions_string()
		));
	}

	// After compiling a program, bind the inputs from the extra state into the program
	void bind_inputs(std::vector<std::shared_ptr<shadertoy::bound_inputs_base>> &inputs,
		shadertoy::gl::program &program) override
	{
		inputs.push_back(extra_inputs_.bind_inputs(program));
	}

public:
	// Extra state accessor
	example_inputs_t &extra_inputs() { return extra_inputs_; }

	example_render_context(shadertoy::context_config &config)
		: shadertoy::render_context(config),
		extra_inputs_()
	{
		// Add a custom runtime input
		extra_inputs_.get<iDynamicFloats>().insert<float>("iCustomTime", 0.0f);
	}
};

void set_framebuffer_size(GLFWwindow *window, int width, int height)
{
	// Get the context from the window user pointer
	shadertoy::render_context &context =
		*static_cast<shadertoy::render_context*>(glfwGetWindowUserPointer(window));

	// Reallocate textures
	context.config().width = width;
	context.config().height = height;
	context.allocate_textures();
}

int main(int argc, char *argv[])
{
	int code = 0;

	shadertoy::context_config contextConfig;
	contextConfig.width = 640;
	contextConfig.height = 480;
	contextConfig.target_framerate = 60.0;

	shadertoy::buffer_config imageBuffer;
	imageBuffer.name = "image";
	imageBuffer.shader_files.push_back(fs::path("..") / fs::path("shaders") / fs::path("shader-gradient-uniform.glsl"));

	contextConfig.buffer_configs.insert(make_pair(imageBuffer.name,
												 imageBuffer));

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 1;
	}

	// Initialize window
	GLFWwindow *window = glfwCreateWindow(contextConfig.width,
										  contextConfig.height,
										  "libshadertoy example 15-uniforms",
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
			example_render_context context(contextConfig);
			std::cout << "Created context based on config" << std::endl;

			try
			{
				// Initialize context
				context.init();
				std::cout << "Initialized rendering context" << std::endl;
			}
			catch (shadertoy::gl::shader_compilation_error &sce)
			{
				std::cerr << "Failed to compile shader: " << sce.log();
				code = 2;
			}
			catch (shadertoy::shadertoy_error &err)
			{
				std::cerr << "Error: "
						  << err.what();
				code = 2;
			}

			if (code)
				exit(code);

			// Now render for 5s
			int frameCount = 0;
			double t = 0.;

			// Set the resize callback
			glfwSetWindowUserPointer(window, &context);
			glfwSetFramebufferSizeCallback(window, set_framebuffer_size);

			while (!glfwWindowShouldClose(window))
			{
				// Poll events
				glfwPollEvents();

				// Update uniforms
				context.state().get<shadertoy::iTime>() = t;
				context.state().get<shadertoy::iFrame>() = frameCount;

				// Update custom uniform
				context.extra_inputs().get<iDynamicFloats>().get<float>("iCustomTime") = (int(t) % 2) == 0 ? 1.0f : 0.0f;

				// Render to texture
				context.render();

				// Render to screen
				//  Setup framebuffer
				gl_call(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);
				gl_call(glViewport, 0, 0, contextConfig.width, contextConfig.height);

				//  Load texture and program
				context.bind_result();

				//  Render result
				context.render_screen_quad();

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

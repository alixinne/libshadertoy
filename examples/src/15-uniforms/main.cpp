#include <epoxy/gl.h>

#include <GLFW/glfw3.h>

#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>

#include "uniforms.hpp"
#include "test.hpp"

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

	// After compiling a program, bind the inputs from the extra state into the program
	void bind_inputs(std::vector<std::shared_ptr<shadertoy::bound_inputs_base>> &inputs,
		const shadertoy::gl::program &program) const override
	{
		inputs.push_back(extra_inputs_.bind_inputs(program));
	}

public:
	// Extra state accessor
	example_inputs_t &extra_inputs() { return extra_inputs_; }

	example_render_context() : shadertoy::render_context(), extra_inputs_()
	{
		// Add a custom runtime input
		extra_inputs_.get<iDynamicFloats>().insert<float>("iCustomTime", 0.0f);

		// Update the template
		buffer_template().insert_after("shadertoy:uniforms",
			shadertoy::compiler::template_part("example:uniforms", extra_inputs_.definitions_string()));
	}
};

typedef basic_example_ctx<example_render_context> custom_example_ctx;

int main(int argc, char *argv[])
{
	int code = 0;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 1;
	}

	// Initialize window
	int width = 640, height = 480;
	GLFWwindow *window = glfwCreateWindow(width, height, "libshadertoy example 15-uniforms", nullptr, nullptr);

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
			custom_example_ctx ctx;
			auto &context(ctx.context);
			auto &chain(ctx.chain);

			// Set the context parameters (render size and some uniforms)
			ctx.render_size = shadertoy::rsize(width, height);
			context.state().get<shadertoy::iTimeDelta>() = 1.0 / 60.0;
			context.state().get<shadertoy::iFrameRate>() = 60.0;

			// Create the image buffer
			auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image"));
			imageBuffer->source_files().push_back("../shaders/shader-gradient-uniform.glsl");

			// Add the image buffer to the swap chain
			chain.emplace_back(imageBuffer, shadertoy::make_size_ref(ctx.render_size));

			// Create a swap chain member that renders to the screen
			chain.push_back(shadertoy::members::make_screen(shadertoy::make_size_ref(ctx.render_size)));

			try
			{
				// Initialize context
				context.init(chain);
				std::cout << "Initialized swap chain" << std::endl;
			}
			catch (shadertoy::gl::shader_compilation_error &sce)
			{
				std::cerr << "Failed to compile shader: " << sce.log();
				code = 2;
			}
			catch (shadertoy::shadertoy_error &err)
			{
				std::cerr << "Error: " << err.what();
				code = 2;
			}

			if (code)
				exit(code);

			// Now render for 5s
			int frameCount = 0;
			double t = 0.;

			// Set the resize callback
			glfwSetWindowUserPointer(window, &ctx);
			glfwSetFramebufferSizeCallback(window, example_set_framebuffer_size<custom_example_ctx>);

			while (!glfwWindowShouldClose(window))
			{
				// Poll events
				glfwPollEvents();

				// Update uniforms
				context.state().get<shadertoy::iTime>() = t;
				context.state().get<shadertoy::iFrame>() = frameCount;

				// Update custom uniform
				context.extra_inputs().get<iDynamicFloats>().get<float>("iCustomTime") = (int(t) % 2) == 0 ? 1.0f : 0.0f;

				// Render the swap chain
				context.render(chain);

				// Buffer swapping
				glfwSwapBuffers(window);

				// Update time and framecount
				t = glfwGetTime();
				frameCount++;

				if (libshadertoy_test_exit())
					glfwSetWindowShouldClose(window, true);
			}
		}

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}

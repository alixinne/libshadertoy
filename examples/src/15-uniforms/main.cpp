#include <epoxy/gl.h>

#include <GLFW/glfw3.h>

#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>
#include <shadertoy/utils/log.hpp>

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

int main(int argc, char *argv[])
{
	int code = 0;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 2;
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

		shadertoy::utils::log::shadertoy()->set_level(spdlog::level::trace);

		try
		{
			example_ctx ctx;
			auto &context(ctx.context);
			auto &chain(ctx.chain);

			// Extra uniform inputs storage
			example_inputs_t extra_inputs;

			// Add a custom runtime input
			// This needs to happen before compiling shaders relying on these inputs,
			// otherwise the generated sources will not include the corresponding uniform
			// definition.
			extra_inputs.get<iDynamicFloats>().insert<float>("iCustomTime", 0.0f);

			// Update the template to include the inputs when binding new programs
			context.buffer_template().shader_inputs().push_back(&extra_inputs);

			// Update the template to include the generated sources for the extra uniforms
			context.buffer_template()[GL_FRAGMENT_SHADER].insert_after("shadertoy:uniforms",
				shadertoy::compiler::template_part("example:uniforms", extra_inputs.definitions_string()));

			// Set the internal format of the chain
			chain.internal_format(GL_RGB8);
			chain.swap_policy(shadertoy::member_swap_policy::single_buffer);

			// Set the context parameters (render size and some uniforms)
			ctx.render_size = shadertoy::rsize(width, height);
			context.state().get<shadertoy::iTimeDelta>() = 1.0 / 60.0;
			context.state().get<shadertoy::iFrameRate>() = 60.0;

			// Create the image buffer
			auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image"));
			imageBuffer->source_file(ST_BASE_DIR "/shaders/shader-gradient-uniform.glsl");

			// Add the image buffer to the swap chain
			chain.emplace_back(imageBuffer, shadertoy::make_size_ref(ctx.render_size));

			// Create a swap chain member that renders to the screen
			chain.emplace_back<shadertoy::members::screen_member>(shadertoy::make_size_ref(ctx.render_size));

			// Initialize context
			context.init(chain);
			std::cout << "Initialized swap chain" << std::endl;

			// Now render for 5s
			int frameCount = 0;
			double t = 0.;

			// Set the resize callback
			glfwSetWindowUserPointer(window, &ctx);
			glfwSetFramebufferSizeCallback(window, example_set_framebuffer_size<example_ctx>);

			while (!glfwWindowShouldClose(window))
			{
				// Poll events
				glfwPollEvents();

				// Update uniforms
				context.state().get<shadertoy::iTime>() = t;
				context.state().get<shadertoy::iFrame>() = frameCount;

				// Update custom uniform
				extra_inputs.get<iDynamicFloats>().get<float>("iCustomTime") = (int(t) % 2) == 0 ? 1.0f : 0.0f;

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

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}

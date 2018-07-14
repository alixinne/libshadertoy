#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>
#include <shadertoy/utils/log.hpp>

#include "test.hpp"

namespace fs = boost::filesystem;
using shadertoy::gl::gl_call;

int main(int argc, char *argv[])
{
	int code = 0;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 2;
	}

	// Initialize window
	int width = 7 * 64, height = 7 * 64;
	GLFWwindow *window = glfwCreateWindow(width, height, "libshadertoy example 11-image", nullptr, nullptr);

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

			// Set the context parameters (render size and some uniforms)
			ctx.render_size = shadertoy::rsize(width, height);
			context.state().get<shadertoy::iTimeDelta>() = 1.0 / 60.0;
			context.state().get<shadertoy::iFrameRate>() = 60.0;

			// Create an auxiliary buffer that renders a gradient
			auto gradientBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("gradient"));
			gradientBuffer->source_file(ST_BASE_DIR "/shaders/shader-gradient.glsl");
			auto buffer_input(std::make_shared<shadertoy::inputs::buffer_input>(chain.emplace_back(gradientBuffer, shadertoy::make_size(shadertoy::rsize(16, 16)), GL_R8)));

			// Create the image buffer
			auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image"));
			imageBuffer->source_file(ST_BASE_DIR "/shaders/shader-image.glsl");

			shadertoy::utils::input_loader loader;

			imageBuffer->inputs().emplace_back(loader.create("file:///" ST_BASE_DIR "/images/vase_rect.exr", false)); // implicit iChannel0
			imageBuffer->inputs().emplace_back(loader.create("file:///" ST_BASE_DIR "/images/vase_rect.png", false)); // implicit iChannel1
			imageBuffer->inputs().emplace_back(loader.create("file:///" ST_BASE_DIR "/images/vase_rect.jpg", false)); // implicit iChannel2
			imageBuffer->inputs().emplace_back("noiseChannel", loader.create("noise:?width=64&height=64"));
			imageBuffer->inputs().emplace_back("checkerChannel", loader.create("checker:?width=64&height=64&size=4"));
			imageBuffer->inputs().emplace_back("errorChannel", context.error_input());
			imageBuffer->inputs().emplace_back("bufferChannel", buffer_input);

			for (auto &program_input : imageBuffer->inputs())
			{
				program_input.input()->mag_filter(GL_LINEAR);
				program_input.input()->min_filter(GL_LINEAR_MIPMAP_LINEAR);
				program_input.input()->wrap(GL_REPEAT);
			}

			imageBuffer->inputs().back().input()->sampler().parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

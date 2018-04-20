#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>

#include "test.hpp"

namespace fs = boost::filesystem;
using shadertoy::gl::gl_call;

typedef struct
{
	shadertoy::render_context context;
	shadertoy::swap_chain chain;
} example_ctx;

void set_framebuffer_size(GLFWwindow *window, int width, int height)
{
	// Get the context from the window user pointer
	auto &ctx = *static_cast<example_ctx *>(glfwGetWindowUserPointer(window));

	// Reallocate textures
	ctx.context.render_size(shadertoy::rsize(width, height));
	ctx.context.allocate_textures(ctx.chain);
}

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
	GLFWwindow *window = glfwCreateWindow(width, height, "libshadertoy example 10-gradient", nullptr, nullptr);

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
			example_ctx ctx;
			auto &context(ctx.context);
			auto &chain(ctx.chain);

			// Set the context parameters (render size and some uniforms)
			context.render_size(shadertoy::rsize(width, height));
			context.state().get<shadertoy::iTimeDelta>() = 1.0 / 60.0;
			context.state().get<shadertoy::iFrameRate>() = 60.0;

			// Create the image buffer
			auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image"));
			imageBuffer->render_size(shadertoy::rsize_ref([&context](){ return context.render_size(); }));
			imageBuffer->source_files().push_back("../shaders/shader-gradient.glsl");

			// Add the image buffer to the swap chain
			chain.emplace_back(imageBuffer);

			// Create a swap chain member that renders to the screen
			auto screenRender(std::make_shared<shadertoy::members::screen_member>());
			screenRender->viewport_size(shadertoy::rsize_ref([&context](){ return context.render_size(); }));
			
			// Add it to the swap chain
			chain.push_back(screenRender);

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
			glfwSetFramebufferSizeCallback(window, set_framebuffer_size);

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

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}

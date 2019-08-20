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

	glfwSetErrorCallback([] (int error, const char *description) {
						 std::cerr << "GLFW error: " << description << std::endl;
						 });

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Initialize window
	int width = 640, height = 480;
	GLFWwindow *window = glfwCreateWindow(width, height, "libshadertoy example - uniforms", nullptr, nullptr);

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

			// Set the internal format of the chain
			chain.internal_format(GL_RGB8);
			chain.swap_policy(shadertoy::member_swap_policy::single_buffer);

			// Set the context parameters (render size and some uniforms)
			ctx.render_size = shadertoy::rsize(width, height);

			// Create the image buffer
			auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image"));
			imageBuffer->source_file(ST_BASE_DIR "/uniforms/gradient-uniform.glsl");

			// Add the image buffer to the swap chain
			chain.emplace_back(imageBuffer, shadertoy::make_size_ref(ctx.render_size));

			// Create a swap chain member that renders to the screen
			chain.emplace_back<shadertoy::members::screen_member>(shadertoy::make_size_ref(ctx.render_size));

			// Initialize context
			context.init(chain);
			std::cout << "Initialized swap chain" << std::endl;

			// Set uniforms
			chain.set_uniform("iTimeDelta", 1.0f / 60.0f);
			chain.set_uniform("iFrameRate", 60.0f);

			// Now render for 5s
			int frameCount = 0;
			float t = 0.;

			// Set the resize callback
			glfwSetWindowUserPointer(window, &ctx);
			glfwSetFramebufferSizeCallback(window, example_set_framebuffer_size<example_ctx>);

			while (!glfwWindowShouldClose(window))
			{
				// Poll events
				glfwPollEvents();

				// Update uniforms
				chain.set_uniform("iTime", t);
				chain.set_uniform("iFrame", frameCount);

				// Update custom uniform
				chain.set_uniform("iCustomTime", (int(t) % 2) == 0 ? 1.0f : 0.0f);

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

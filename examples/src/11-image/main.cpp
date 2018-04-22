#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>

#include "test.hpp"

namespace fs = boost::filesystem;
using shadertoy::gl::gl_call;

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

		{
			example_ctx ctx;
			auto &context(ctx.context);
			auto &chain(ctx.chain);

			// Set the context parameters (render size and some uniforms)
			ctx.render_size = shadertoy::rsize(width, height);
			context.state().get<shadertoy::iTimeDelta>() = 1.0 / 60.0;
			context.state().get<shadertoy::iFrameRate>() = 60.0;

			// Create the image buffer
			auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image"));
			imageBuffer->source_files().push_back("../shaders/shader-image.glsl");

			imageBuffer->inputs()[1] = std::make_shared<shadertoy::inputs::soil_input>("../images/vase_rect.png");
			imageBuffer->inputs()[2] = std::make_shared<shadertoy::inputs::jpeg_input>("../images/vase_rect.jpg");
			imageBuffer->inputs()[3] = std::make_shared<shadertoy::inputs::error_input>();

#if LIBSHADERTOY_OPENEXR
			imageBuffer->inputs()[0] = std::make_shared<shadertoy::inputs::exr_input>("../images/vase_rect.exr");
#else
			imageBuffer->inputs()[0] = std::make_shared<shadertoy::inputs::error_input>();
#endif
			for (size_t i = 0; i < 3; ++i)
			{
				imageBuffer->inputs()[i]->mag_filter(GL_LINEAR);
				imageBuffer->inputs()[i]->min_filter(GL_LINEAR_MIPMAP_LINEAR);
				imageBuffer->inputs()[i]->wrap(GL_REPEAT);
			}

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

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}

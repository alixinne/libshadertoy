#include <epoxy/gl.h>

#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>
#include <shadertoy/backends/gl4.hpp>
#include <shadertoy/utils/log.hpp>

#include "test.hpp"

namespace fs = boost::filesystem;

int main(int argc, char *argv[])
{
	int code = 0;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 2;
	}

	glfwSetErrorCallback([](int error, const char *description) {
		std::cerr << "GLFW error: " << description << std::endl;
	});

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Initialize window
	int width = 7 * 64, height = 7 * 64;
	GLFWwindow *window = glfwCreateWindow(width, height, "libshadertoy example - image", nullptr, nullptr);

	if (!window)
	{
		std::cerr << "Failed to create glfw window" << std::endl;
		code = 1;
	}
	else
	{
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		// Set the backend to raw OpenGL 4
		shadertoy::backends::set_current(std::make_unique<shadertoy::backends::gl4::backend>());

		shadertoy::utils::log::shadertoy()->set_level(spdlog::level::trace);

		try
		{
			example_ctx ctx;
			auto &context(ctx.context);
			auto &chain(ctx.chain);

			// Set the context parameters (render size and some uniforms)
			ctx.render_size = shadertoy::rsize(width, height);

			// Create an auxiliary buffer that renders a gradient
			auto gradientBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("gradient"));
			gradientBuffer->source_file(ST_BASE_DIR "/image/image-store.glsl");

			// Add image load/store image target
			auto texture(shadertoy::backends::current()->make_texture(GL_TEXTURE_2D));
			texture->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST); // no mipmaps
			texture->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			auto gradient_image(std::make_shared<shadertoy::inputs::texture_input>(std::move(texture), GL_RGBA32F));
			gradientBuffer->inputs().emplace_back("layout(rgba32f)", "image2D", "gradientImage", gradient_image);

			auto buffer_input(std::make_shared<shadertoy::inputs::buffer_input>(
			chain.emplace_back(gradientBuffer, shadertoy::make_size(16u, 16u), GL_R8)));

			// Create the image buffer
			auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image"));
			imageBuffer->source_file(ST_BASE_DIR "/image/display.glsl");

			// clang-format off

			imageBuffer->inputs().emplace_back(std::make_shared<shadertoy::inputs::openimageio_input>(
				ST_BASE_DIR "/image/vase_rect.exr")); // implicit iChannel0
			imageBuffer->inputs().emplace_back(std::make_shared<shadertoy::inputs::openimageio_input>(
				ST_BASE_DIR "/image/vase_rect.png")); // implicit iChannel1
			imageBuffer->inputs().emplace_back(std::make_shared<shadertoy::inputs::openimageio_input>(
				ST_BASE_DIR "/image/vase_rect.jpg")); // implicit iChannel2
			imageBuffer->inputs().emplace_back("noiseChannel", std::make_shared<shadertoy::inputs::noise_input>(
				shadertoy::make_size(64u, 64u)));
			imageBuffer->inputs().emplace_back("checkerChannel", std::make_shared<shadertoy::inputs::checker_input>(
				shadertoy::make_size(64u, 64u), 4));
			imageBuffer->inputs().emplace_back("errorChannel", context.error_input());

			// clang-format on

			auto program_buffer_input(imageBuffer->inputs().emplace_back("bufferChannel", buffer_input));
			auto program_image_input(imageBuffer->inputs().emplace_back("imageChannel", gradient_image));

			for (auto &program_input : imageBuffer->inputs())
			{
				program_input.input()->mag_filter(GL_LINEAR);
				program_input.input()->min_filter(GL_LINEAR_MIPMAP_LINEAR);
				program_input.input()->wrap(GL_REPEAT);
			}

			program_buffer_input.input()->sampler().parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			program_image_input.input()->min_filter(GL_LINEAR);

			// Add the image buffer to the swap chain
			auto image_member(chain.emplace_back(imageBuffer, shadertoy::make_size_ref(ctx.render_size)));

			// Image load store is not coherent
			image_member->state().memory_barrier(GL_TEXTURE_FETCH_BARRIER_BIT);

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
		catch (shadertoy::backends::gx::shader_compilation_error &sce)
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

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
	int width = 640, height = 480;
	GLFWwindow *window = glfwCreateWindow(width, height, "libshadertoy example - compute", nullptr, nullptr);

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
		shadertoy::backends::current = std::make_unique<shadertoy::backends::gl4::backend>();

		shadertoy::utils::log::shadertoy()->set_level(spdlog::level::trace);

		try
		{
			example_ctx ctx;
			auto &context(ctx.context);
			auto &chain(ctx.chain);

			// Set the context parameters (render size and some uniforms)
			ctx.render_size = shadertoy::rsize(width, height);

			// Create a simple texture for the compute buffer
			int tex_width = 16;
			auto texture(shadertoy::backends::current->make_texture(GL_TEXTURE_1D));
			texture->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST); // no mipmaps
			texture->image_1d(GL_TEXTURE_1D, 0, GL_R32F, tex_width, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

			// Turn the texture into an input
			auto compute_image(std::make_shared<shadertoy::inputs::texture_input>(std::move(texture), GL_R32F));

			// Create the compute pass
			auto compute_buffer(std::make_shared<shadertoy::buffers::compute_buffer>("compute"));
			compute_buffer->source_file(ST_BASE_DIR "/compute/compute.glsl");

			// Set the dispatch size
			compute_buffer->num_groups(glm::uvec3(tex_width, 1, 1));

			// Add the image input
			compute_buffer->inputs().emplace_back("layout(r32f)", "image1D", "computeImage", compute_image);

			// Add the compute buffer to the swap chain
			chain.emplace_back(compute_buffer);

			// Create the image buffer
			auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image"));
			imageBuffer->source_file(ST_BASE_DIR "/compute/display.glsl");

			// Add the compute image input to the final shader pass
			imageBuffer->inputs().emplace_back("sampler1D", "computeImage", compute_image);

			// Add the image buffer to the swap chain, at the given size
			// The default_framebuffer policy makes this buffer draw directly to
			// the window instead of using a texture that is then copied to the
			// screen.
			auto image_member(chain.emplace_back(imageBuffer, shadertoy::make_size_ref(ctx.render_size),
												 shadertoy::member_swap_policy::default_framebuffer));

			// Image load store is not coherent
			image_member->state().memory_barrier(GL_TEXTURE_FETCH_BARRIER_BIT);

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

				// Set viewport
				// This is not necessary when the last pass is rendering to a
				// texture and it is followed by a screen_member, which calls
				// glViewport. In this example, we render directly to the
				// default framebuffer, so we need to set the viewport
				// ourselves.
				shadertoy::backends::current->set_viewport(0, 0, ctx.render_size.width,
														   ctx.render_size.height);

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

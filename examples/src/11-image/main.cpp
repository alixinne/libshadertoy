#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>

namespace fs = boost::filesystem;
using shadertoy::gl::gl_call;

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
	imageBuffer.shader_files.push_back(fs::path("..") / fs::path("shaders") / fs::path("shader-image.glsl"));

	imageBuffer.inputs[0].id = "image.0";
	imageBuffer.inputs[0].type = "texture";
#if LIBSHADERTOY_OPENEXR
	imageBuffer.inputs[0].source = fs::path("../images/vase_rect.exr").string();
#else /* LIBSHADERTOY_OPENEXR */
	imageBuffer.inputs[0].source = fs::path("../images/vase_rect.png").string();
#endif /* LIBSHADERTOY_OPENEXR */

	contextConfig.buffer_configs.emplace_back(imageBuffer.name, imageBuffer);

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 1;
	}

	// Initialize window
	GLFWwindow *window = glfwCreateWindow(contextConfig.width,
										  contextConfig.height,
										  "libshadertoy example 11-image",
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
			shadertoy::render_context context(contextConfig);
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

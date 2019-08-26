#include <GLES3/gl3.h>

#include <shadertoy.hpp>
#include <shadertoy/backends/webgl.hpp>

#include <GLFW/glfw3.h>
#include <emscripten.h>

struct ctx
{
	shadertoy::render_context context;
	shadertoy::swap_chain chain;
	shadertoy::rsize render_size;
	GLFWwindow *window;
	int frameCount;
	float t = 0.;
	std::unique_ptr<shadertoy::backends::gx::backend> backend;

	ctx(GLFWwindow *window) : window(window), frameCount(0), t(0.)
	{
		puts("shadertoy context created!");
	}

	~ctx() { puts("shadertoy context destroyed"); }

	void render()
	{
		try
		{
			if (backend)
			{
				shadertoy::backends::current = std::move(backend);
			}

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
			shadertoy::backends::current->set_viewport(0, 0, render_size.width, render_size.height);

			// Render the swap chain
			context.render(chain);

			// Buffer swapping
			glfwSwapBuffers(window);

			// Update time and framecount
			t = glfwGetTime();
			frameCount++;
		}
		catch (shadertoy::backends::gx::program_validate_error &ex)
		{
			fprintf(stderr, "failed to validate program: %s\n", ex.log().c_str());
			abort();
		}
		catch (shadertoy::shadertoy_error &ex)
		{
			fprintf(stderr, "generic error: %s\n", ex.what());
			abort();
		}
	}
};

std::unique_ptr<ctx> context;

void render() { context->render(); }

int main()
{
	try
	{
		if (!glfwInit())
		{
			puts("Could not initialize GLFW\n");
			return 1;
		}

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

		int width = 640, height = 480;
		auto window = glfwCreateWindow(width, height, "shadertoy web-gradient", NULL, NULL);
		if (window == NULL)
		{
			puts("Could not create window\n");
			glfwTerminate();
			return 1;
		}

		glfwMakeContextCurrent(window);
		shadertoy::backends::current = std::make_unique<shadertoy::backends::webgl::backend>();

		context = std::make_unique<ctx>(window);

		// Set the render size
		context->render_size = shadertoy::rsize(width, height);

		auto &ctx(context->context);
		auto &chain(context->chain);

		// Create the image buffer
		// WebGL note: we specify "1" as the expected number of outputs for this buffer,
		// since we don't have program interface introspection on WebGL (thus we can't
		// detect outputs).
		auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image", 1));
		imageBuffer->source_file("gradient.glsl");

		// Add the image buffer to the swap chain, at the given size The
		// default_framebuffer policy makes this buffer draw directly to the
		// window instead of using a texture that is then copied to the screen.
		chain.emplace_back(imageBuffer, shadertoy::make_size_ref(context->render_size),
						   shadertoy::member_swap_policy::default_framebuffer);

		// Initialize context
		ctx.init(chain);

		// Set uniforms
		chain.set_uniform("iTimeDelta", 1.0f / 60.0f);
		chain.set_uniform("iFrameRate", 60.0f);

		// Set the resize callback
		// glfwSetFramebufferSizeCallback(window, resize_callback);

		// Note: render loop changes threads
		context->backend = std::move(shadertoy::backends::current);
		emscripten_set_main_loop(render, 0, 0);
	}
	catch (shadertoy::backends::gx::shader_compilation_error &ex)
	{
		fprintf(stderr, "failed to compile shader: %s\n", ex.log().c_str());
	}
	catch (shadertoy::backends::gx::program_link_error &ex)
	{
		fprintf(stderr, "failed to link program: %s\n", ex.log().c_str());
	}
	catch (shadertoy::backends::gx::program_validate_error &ex)
	{
		fprintf(stderr, "failed to validate program: %s\n", ex.log().c_str());
	}
	catch (shadertoy::shadertoy_error &ex)
	{
		fprintf(stderr, "generic error: %s\n", ex.what());
	}

	/* Disabled because emscripten_set_main_loop is non-blocking and happens
	   in another thread */
	//context.reset();
	//glfwDestroyWindow(context->window);
	//glfwTerminate();

	return 0;
}

#include <GLES3/gl3.h>

#include <shadertoy.hpp>
#include <shadertoy/backends/webgl.hpp>
#include <shadertoy/utils/log.hpp>

#include <GLFW/glfw3.h>
#include <emscripten.h>

#include <fstream>
#include <streambuf>

struct ctx
{
	shadertoy::render_context context;
	shadertoy::swap_chain chain;
	shadertoy::rsize render_size;
	GLFWwindow *window;
	int frameCount;
	float t = 0.;
	float t_offset = 0.;
	std::unique_ptr<shadertoy::backends::gx::backend> backend;
	std::string shader_source;

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
				shadertoy::backends::set_current(std::move(backend));
			}

			// Poll events
			glfwPollEvents();

			// Update uniforms
			chain.set_uniform("iTime", t + t_offset);
			chain.set_uniform("iFrame", frameCount);
			chain.set_uniform("iTimeDelta", 1.0f / 60.0f);
			chain.set_uniform("iFrameRate", 60.0f);

			// Set viewport
			// This is not necessary when the last pass is rendering to a
			// texture and it is followed by a screen_member, which calls
			// glViewport. In this example, we render directly to the
			// default framebuffer, so we need to set the viewport
			// ourselves.
			shadertoy::backends::current()->set_viewport(0, 0, render_size.width, render_size.height);

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

	void reset()
	{
		t_offset = -t;
		frameCount = 0;
	}

	void load_source(const char *src)
	{
		shader_source.assign(src);
		shadertoy::sources::set_source(
		*std::static_pointer_cast<shadertoy::buffers::toy_buffer>(
		std::static_pointer_cast<shadertoy::members::buffer_member>(chain.members().front())->buffer()),
		context.buffer_template(), GL_FRAGMENT_SHADER, shader_source);

		// Reload programs
		context.init(chain);
	}

	const char *get_source() const { return shader_source.c_str(); }
};

std::unique_ptr<ctx> context;

extern "C"
{
	void reset() { context->reset(); }
	void load_source(const char *src) { context->load_source(src); }
	const char *get_source() { return context->get_source(); }
}

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
		shadertoy::backends::set_current(std::make_unique<shadertoy::backends::webgl::backend>());

		shadertoy::utils::log::shadertoy()->set_level(spdlog::level::trace);

		context = std::make_unique<ctx>(window);

		// Set the render size
		context->render_size = shadertoy::rsize(width, height);

		std::ifstream t("gradient.glsl");
		t.seekg(0, std::ios::end);
		context->shader_source.reserve(t.tellg());
		t.seekg(0, std::ios::beg);
		context->shader_source.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

		auto &ctx(context->context);
		auto &chain(context->chain);

		// Create the image buffer
		// WebGL note: we specify "1" as the expected number of outputs for this buffer,
		// since we don't have program interface introspection on WebGL (thus we can't
		// detect outputs).
		auto imageBuffer(std::make_shared<shadertoy::buffers::toy_buffer>("image", 1));
		shadertoy::sources::set_source(*imageBuffer, ctx.buffer_template(), GL_FRAGMENT_SHADER,
									   context->shader_source);

		// Add the image buffer to the swap chain, at the given size The
		// default_framebuffer policy makes this buffer draw directly to the
		// window instead of using a texture that is then copied to the screen.
		chain.emplace_back(imageBuffer, shadertoy::make_size_ref(context->render_size),
						   shadertoy::member_swap_policy::default_framebuffer);

		// Initialize context
		ctx.init(chain);

		// Set the resize callback
		// glfwSetFramebufferSizeCallback(window, resize_callback);

		// Note: render loop changes threads
		context->backend = shadertoy::backends::set_current({});
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

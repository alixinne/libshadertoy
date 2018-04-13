#include <iostream>
#include <string>
#include <sstream>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "api.hpp"
#include "test.hpp"

using namespace std;
using shadertoy::gl::gl_call;

namespace fs = boost::filesystem;
namespace po = boost::program_options;
namespace u = shadertoy::utils;

struct my_context
{
	shadertoy::render_context *context;
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// Close on ESC key press
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// Get the context from the window user pointer
	my_context &ctx = *static_cast<my_context*>(glfwGetWindowUserPointer(window));

	// Reallocate textures
	ctx.context->config().width = width;
	ctx.context->config().height = height;
	ctx.context->allocate_textures();
}

int parse_options(string &shaderId, string &shaderApiKey, bool &dump, int argc, char **argv)
{
	// Read and parse options
	bool help;
	po::options_description desc("libshadertoy 50-shadertoy example");
	desc.add_options()
		("help,h", po::bool_switch(&help)->default_value(false), "show a help message")
		("dump,d", po::bool_switch(&dump)->default_value(false), "Dump binary formats of the loaded programs")
		("id,i", po::value<string>(&shaderId)->required(), "ID of the ShaderToy to render")
		("api,a", po::value<string>(&shaderApiKey)->required(), "API key for ShaderToy query");

	po::variables_map vm;

	try
	{
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		po::notify(vm);
	}
	catch (po::error &ex)
	{
		u::log::shadertoy()->critical(ex.what());
		return 2;
	}

	if (help)
	{
		cout << desc << endl;
		return 1;
	}

	return 0;
}

int render(GLFWwindow* window, shadertoy::context_config &contextConfig, bool dumpShaders)
{
	int code = 0;

	shadertoy::render_context context(contextConfig);
	auto &state(context.state());
	my_context ctx = {&context};

	try
	{
		// Initialize context
		context.init();
		u::log::shadertoy()->info("Initialized rendering context");

		if (dumpShaders)
		{
			for (auto &bufferConfig : contextConfig.buffer_configs)
			{
				auto &firstPath(bufferConfig.second.shader_files.front());
				auto dumpPath(firstPath.replace_extension(".dump"));

				u::log::shadertoy()->info("Dumping {} to {}", bufferConfig.first, dumpPath);

				std::ofstream ofs(dumpPath.string());
				auto dump(shadertoy::utils::dump_program(context.buffer(bufferConfig.first)->program()));
				ofs.write(dump.data(), dump.size());
				ofs.close();
			}
		}
	}
	catch (shadertoy::gl::shader_compilation_error &sce)
	{
		u::log::shadertoy()->error("Failed to compile shader: {}", sce.log());
		code = 2;
	}
	catch (shadertoy::shadertoy_error &err)
	{
		u::log::shadertoy()->error("{}", err.what());
		code = 2;
	}

	if (code == 0)
	{
		// Now render for 5s
		int frameCount = 0;

		// Set the resize callback
		glfwSetWindowUserPointer(window, &ctx);

		glfwSetKeyCallback(window, key_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		while (!glfwWindowShouldClose(window))
		{
			// Poll events
			glfwPollEvents();

			// Update uniforms
			//  iTime and iFrame
			state.get<shadertoy::iTime>() += 1.0 / contextConfig.target_framerate;
			state.get<shadertoy::iFrame>() = frameCount;

			//  iDate
			boost::posix_time::ptime dt = boost::posix_time::microsec_clock::local_time();
			state.get<shadertoy::iDate>() = glm::vec4(dt.date().year() - 1,
											dt.date().month(),
											dt.date().day(),
											dt.time_of_day().total_nanoseconds() / 1e9f);

			//  iMouse
			int btnstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
			if (btnstate == GLFW_PRESS)
			{
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);

				state.get<shadertoy::iMouse>()[0] =
					state.get<shadertoy::iMouse>()[2] = xpos;
				state.get<shadertoy::iMouse>()[1] =
					state.get<shadertoy::iMouse>()[3] = contextConfig.height - ypos;
			}
			else
			{
				state.get<shadertoy::iMouse>()[2] =
					state.get<shadertoy::iMouse>()[3] = 0.f;
			}
			// End update uniforms

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

			// Print execution time
			auto buffer = context.buffer();

			if (buffer)
			{
				auto renderTime = buffer->elapsed_time();
				std::cerr << "frame time: " << renderTime
						  << "ns fps: " << (1e9 / renderTime)
						  << " mpx/s: " << (contextConfig.width * contextConfig.height / (renderTime / 1e3))
						  << std::endl;
			}

			// Buffer swapping
			glfwSwapBuffers(window);

			// Update time and framecount
			frameCount++;

			if (libshadertoy_test_exit())
				glfwSetWindowShouldClose(window, true);
		}
	}

	return code;
}

int performRender(shadertoy::context_config &contextConfig, bool dumpShaders)
{
	int code = 0;

	if (!glfwInit())
	{
		u::log::shadertoy()->critical("Failed to initialize glfw");
		return 1;
	}

	// Initialize window
	GLFWwindow *window = glfwCreateWindow(contextConfig.width,
										  contextConfig.height,
										  "libshadertoy example 50-shadertoy",
										  nullptr,
										  nullptr);

	if (!window)
	{
		u::log::shadertoy()->critical("Failed to create glfw window");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	code = render(window, contextConfig, dumpShaders);

	glfwDestroyWindow(window);
	glfwTerminate();
	return code;
}

int main(int argc, char *argv[])
{
	int code = 0;

	// Options
	string shaderId, shaderApiKey;
	bool dumpShaders;

	// Parse options from ARGV
	code = parse_options(shaderId, shaderApiKey, dumpShaders, argc, argv);
	if (code > 0)
		return code;

	// Context configuration
	shadertoy::context_config contextConfig;
	contextConfig.width = 640;
	contextConfig.height = 480;
	contextConfig.target_framerate = 60.0;

	// Fetch shader code
	code = load_remote(contextConfig, shaderId, shaderApiKey);
	if (code != 0)
		return code;

	// Render
	code = performRender(contextConfig, dumpShaders);

	return code;
}

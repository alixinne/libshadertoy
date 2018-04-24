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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// Close on ESC key press
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
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

int render(GLFWwindow *window, example_ctx &ctx, bool dumpShaders)
{
	int code = 0;

	auto &chain(ctx.chain);
	auto &context(ctx.context);
	auto &state(context.state());

	// Add member that renders to the screen
	chain.push_back(shadertoy::members::make_screen(shadertoy::make_size_ref(ctx.render_size)));

	try
	{
		// Initialize context
		context.init(chain);
		u::log::shadertoy()->info("Initialized rendering context");

		if (dumpShaders)
		{
			for (auto &member : chain.members())
			{
				if (auto buffer_member = std::dynamic_pointer_cast<shadertoy::members::buffer_member>(member))
				{
					auto buffer(std::static_pointer_cast<shadertoy::buffers::toy_buffer>(buffer_member->buffer()));

					auto &firstPath(buffer->source_files().front());
					auto dumpPath(firstPath + std::string(".dump"));

					u::log::shadertoy()->info("Dumping {} to {}", buffer->id(), dumpPath);

					auto dump(shadertoy::utils::dump_program(buffer->program()));
					std::ofstream ofs(dumpPath, std::ios::out | std::ios::binary);
					ofs.write(dump.data(), dump.size());
					ofs.close();
				}
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
		glfwSetFramebufferSizeCallback(window, example_set_framebuffer_size<example_ctx>);

		while (!glfwWindowShouldClose(window))
		{
			// Poll events
			glfwPollEvents();

			// Update uniforms
			//  iTime and iFrame
			state.get<shadertoy::iTime>() += 1.0 / state.get<shadertoy::iFrameRate>();
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
					state.get<shadertoy::iMouse>()[3] = ctx.render_size.height - ypos;
			}
			else
			{
				state.get<shadertoy::iMouse>()[2] =
					state.get<shadertoy::iMouse>()[3] = 0.f;
			}
			// End update uniforms

			// Render to texture
			context.render(chain);

			// Print execution time
			auto buffer = std::static_pointer_cast<shadertoy::buffers::toy_buffer>(std::static_pointer_cast<shadertoy::members::buffer_member>(chain.before(chain.members().back().get()))->buffer());

			auto renderTime = buffer->elapsed_time();
			std::cerr << "frame time: " << renderTime
					  << "ns fps: " << (1e9 / renderTime)
					  << " mpx/s: " << (ctx.render_size.width * ctx.render_size.height / (renderTime / 1e3))
					  << std::endl;

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

template<typename... Args>
int performRender(bool dumpShaders, Args&&... args)
{
	int code = 0;

	if (!glfwInit())
	{
		u::log::shadertoy()->critical("Failed to initialize glfw");
		return 2;
	}

	// Initialize window
	int width = 640, height = 480;
	GLFWwindow *window = glfwCreateWindow(width, height, "libshadertoy example 50-shadertoy", nullptr, nullptr);

	if (!window)
	{
		u::log::shadertoy()->critical("Failed to create glfw window");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	{
		example_ctx ctx;
		auto &context(ctx.context);

		// Set the context parameters (render size and some uniforms)
		ctx.render_size = shadertoy::rsize(width, height);
		context.state().get<shadertoy::iTimeDelta>() = 1.0 / 60.0;
		context.state().get<shadertoy::iFrameRate>() = 60.0;

		code = load_remote(ctx.context, ctx.chain, ctx.render_size, args...);

		if (code == 0)
			code = render(window, ctx, dumpShaders);
	}

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

	// Load shader code and render
	code = performRender(dumpShaders, shaderId, shaderApiKey);

	return code;
}

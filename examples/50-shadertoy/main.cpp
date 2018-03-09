#include <iostream>
#include <string>
#include <sstream>

#include <curl/curl.h>
#include <json/json.h>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <shadertoy.hpp>

using namespace std;
using shadertoy::gl::gl_call;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

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

size_t curl_write_data(char *buffer, size_t size, size_t nmemb, void *userp)
{
	ostream &ss = *static_cast<ostream*>(userp);
	size_t sz = size * nmemb;
	ss.write(buffer, sz);
	return sz;
}

void file_get(CURL *curl, const string &url, const fs::path &dst)
{
	ofstream ofs(dst.string());

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		throw runtime_error(curl_easy_strerror(res));
	}
}

stringstream curl_get(CURL *curl, const string &url)
{
	stringstream ss;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ss);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		throw runtime_error(curl_easy_strerror(res));
	}

	return ss;
}

Json::Value json_get(CURL *curl, const string &url)
{
	Json::Value result;
	curl_get(curl, url) >> result;
	return result;
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
		BOOST_LOG_TRIVIAL(error) << ex.what();
		return 2;
	}

	if (help)
	{
		cout << desc << endl;
		return 1;
	}

	return 0;
}

int load_remote(const string &shaderId, const string &shaderApiKey,
				shadertoy::context_config &contextConfig)
{
	// Init CURL
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();

	if (!curl)
	{
		BOOST_LOG_TRIVIAL(error) << "Failed to initialize curl.";
		curl_global_cleanup();
		return 1;
	}

	int code = 0;
	try
	{
		string endpoint =
			string("https://www.shadertoy.com/api/v1/shaders/") + shaderId +
			string("?key=") + shaderApiKey;
		BOOST_LOG_TRIVIAL(info) << "Fetching shader info from " << endpoint;
		Json::Value shaderSpec = json_get(curl, endpoint);

		// Check errors from ShaderToy
		if (!shaderSpec["Error"].isNull())
		{
			throw runtime_error(shaderSpec["Error"].asString().c_str());
		}

		ofstream dump(shaderId + string(".json"));
		dump << shaderSpec;
		dump.close();

		// Create buffer configs for each render pass
		for (int i = 0; i < shaderSpec["Shader"]["renderpass"].size(); ++i)
		{
			auto &pass(shaderSpec["Shader"]["renderpass"][i]);

			// Create buffer
			shadertoy::buffer_config imageBuffer;
			imageBuffer.name = pass["name"].asString();
			if (imageBuffer.name.empty())
				imageBuffer.name = pass["type"].asString();

			// Skip if sound buffer
			if (pass["type"].asString().compare("sound") == 0)
			{
				BOOST_LOG_TRIVIAL(warning) << "Skipping unsupported sound shader.";
				continue;
			}

			// Load code
			stringstream sspath;
			sspath << shaderId << "-" << i << ".glsl";
			fs::path p(sspath.str());

			if (!fs::exists(p))
			{
				ofstream ofs(p.string());
				ofs << pass["code"].asString();
				ofs.close();
			}

			// Load inputs
			for (int j = 0; j < pass["inputs"].size(); ++j)
			{
				auto &input(pass["inputs"][j]);
				auto &conf(imageBuffer.inputs[input["channel"].asInt()]);
				auto &sampler(input["sampler"]);

				stringstream ssname;
				ssname << imageBuffer.name << "." << input["channel"].asInt();
				conf.id = ssname.str();

				if (sampler["filter"].compare("mipmap") == 0)
				{
					conf.min_filter = GL_LINEAR_MIPMAP_LINEAR;
					conf.mag_filter = GL_LINEAR;
				}
				else if (sampler["filter"].compare("linear") == 0)
				{
					conf.min_filter = GL_LINEAR;
					conf.mag_filter = GL_LINEAR;
				}
				else if (sampler["filter"].compare("nearest") == 0)
				{
					conf.min_filter = GL_NEAREST;
					conf.mag_filter = GL_NEAREST;
				}

				if (sampler["wrap"].compare("repeat") == 0)
				{
					conf.wrap = GL_REPEAT;
				}
				else if (sampler["wrap"].compare("clamp") == 0)
				{
					conf.wrap = GL_CLAMP_TO_EDGE;
				}

				conf.vflip = sampler["vflip"].compare("true") == 0;

				if (input["ctype"].compare("texture") == 0
					|| input["ctype"].compare("cubemap") == 0)
				{
					conf.type = "texture";

					fs::path srcpath(input["src"].asString());
					string url =
						string("https://www.shadertoy.com")
						+ input["src"].asString();
					fs::path dstpath(srcpath.filename());

					if (!fs::exists(dstpath))
					{
						BOOST_LOG_TRIVIAL(info) << "Downloading " << url;
						file_get(curl, url, dstpath);
					}
					else
					{
						BOOST_LOG_TRIVIAL(info) << "Using cache for " << url;
					}

					conf.source = dstpath.string();
				}
				else if (input["ctype"].compare("buffer") == 0)
				{
					conf.type = "buffer";
					conf.source = "Buf A";
					conf.source.back() = 'A' + (input["id"].asInt() - 257);

					BOOST_LOG_TRIVIAL(debug) <<
						"Pass " << i << ", input " << input["channel"].asInt()
						<< ": binding " << conf.source << " buffer " <<
						conf.enabled();
				}
				else
				{
					stringstream ss;
					ss << "Unsupported input " << input["ctype"].asString()
					   << " for pass " << i << ", input " << input["channel"].asInt();

					BOOST_LOG_TRIVIAL(warning) << ss.str();
				}
			}

			// Add to context
			imageBuffer.shader_files.push_back(p);
			contextConfig.buffer_configs.insert(make_pair(imageBuffer.name,
														 imageBuffer));
		}

		// Image buffer should be last
		pair<string, shadertoy::buffer_config> imagebuf(*contextConfig.buffer_configs.begin());
		contextConfig.buffer_configs.erase(contextConfig.buffer_configs.begin());
		contextConfig.buffer_configs.insert(imagebuf);
	}
	catch (exception &ex)
	{
		BOOST_LOG_TRIVIAL(error) << ex.what();
		code = 1;
	}

	// Free CURL
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return code;
}

int render(GLFWwindow* window, shadertoy::context_config &contextConfig)
{
	int code = 0;

	shadertoy::render_context context(contextConfig);
	auto &state(context.state());
	my_context ctx = {&context};

	try
	{
		// Initialize context
		context.init();
		BOOST_LOG_TRIVIAL(info) << "Initialized rendering context";
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
		}
	}

	return code;
}

int performRender(shadertoy::context_config &contextConfig)
{
	int code = 0;

	if (!glfwInit())
	{
		BOOST_LOG_TRIVIAL(error) << "Failed to initialize glfw";
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
		BOOST_LOG_TRIVIAL(error) << "Failed to create glfw window";
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	code = render(window, contextConfig);

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
	contextConfig.dump_shaders = dumpShaders;

	// Fetch shader code
	code = load_remote(shaderId, shaderApiKey, contextConfig);
	if (code > 0)
		return code;

	// Render
	code = performRender(contextConfig);

	return code;
}

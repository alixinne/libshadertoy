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

#include <shadertoy/Shadertoy.hpp>

using namespace std;
using shadertoy::OpenGL::glCall;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

struct MyContext
{
	shadertoy::RenderContext *context;
};

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// Close on ESC key press
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	// Get the context from the window user pointer
	MyContext &ctx = *static_cast<MyContext*>(glfwGetWindowUserPointer(window));

	// Reallocate textures
	ctx.context->GetConfig().width = width;
	ctx.context->GetConfig().height = height;
	ctx.context->AllocateTextures();
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

int parseOptions(string &shaderId, string &shaderApiKey, bool &dump, int argc, char *argv[])
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

int loadRemote(const string &shaderId, const string &shaderApiKey,
				shadertoy::ContextConfig &contextConfig)
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
			shadertoy::BufferConfig imageBuffer;
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
				auto &conf(imageBuffer.inputConfig[input["channel"].asInt()]);
				auto &sampler(input["sampler"]);

				stringstream ssname;
				ssname << imageBuffer.name << "." << input["channel"].asInt();
				conf.id = ssname.str();

				if (sampler["filter"].compare("mipmap") == 0)
				{
					conf.minFilter = GL_LINEAR_MIPMAP_LINEAR;
					conf.magFilter = GL_LINEAR;
				}
				else if (sampler["filter"].compare("linear") == 0)
				{
					conf.minFilter = GL_LINEAR;
					conf.magFilter = GL_LINEAR;
				}
				else if (sampler["filter"].compare("nearest") == 0)
				{
					conf.minFilter = GL_NEAREST;
					conf.magFilter = GL_NEAREST;
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

					if (!(input["ctype"].compare("keyboard") == 0))
					{
						throw runtime_error(ss.str().c_str());
					}
					else
					{
						BOOST_LOG_TRIVIAL(warning) << ss.str();
					}
				}
			}

			// Add to context
			imageBuffer.shaderFiles.push_back(p);
			contextConfig.bufferConfigs.insert(make_pair(imageBuffer.name,
														 imageBuffer));
		}

		// Image buffer should be last
		pair<string, shadertoy::BufferConfig> imagebuf(*contextConfig.bufferConfigs.begin());
		contextConfig.bufferConfigs.erase(contextConfig.bufferConfigs.begin());
		contextConfig.bufferConfigs.insert(imagebuf);
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

int performRender(shadertoy::ContextConfig &contextConfig)
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

	shadertoy::RenderContext context(contextConfig);
	auto &state(context.GetState());
	MyContext ctx = {&context};

	try
	{
		// Initialize context
		context.Initialize();
		BOOST_LOG_TRIVIAL(info) << "Initialized rendering context";
	}
	catch (shadertoy::OpenGL::ShaderCompilationError &sce)
	{
		std::cerr << "Failed to compile shader: " << sce.log();
		code = 2;
	}
	catch (shadertoy::ShadertoyError &err)
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

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

		while (!glfwWindowShouldClose(window))
		{
			// Poll events
			glfwPollEvents();

			// Update uniforms
			//  iTime and iFrame
			state.V<shadertoy::iTime>() += 1.0 / contextConfig.targetFramerate;
			state.V<shadertoy::iFrame>() = frameCount;

			//  iDate
			boost::posix_time::ptime dt = boost::posix_time::microsec_clock::local_time();
			state.V<shadertoy::iDate>() = glm::vec4(dt.date().year() - 1,
											dt.date().month(),
											dt.date().day(),
											dt.time_of_day().total_nanoseconds() / 1e9f);

			//  iMouse
			int btnstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
			if (btnstate == GLFW_PRESS)
			{
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);

				state.V<shadertoy::iMouse>()[0] =
					state.V<shadertoy::iMouse>()[2] = xpos;
				state.V<shadertoy::iMouse>()[1] =
					state.V<shadertoy::iMouse>()[3] = contextConfig.height - ypos;
			}
			else
			{
				state.V<shadertoy::iMouse>()[2] =
					state.V<shadertoy::iMouse>()[3] = 0.f;
			}
			// End update uniforms

			// Render to texture
			context.Render();

			// Render to screen
			//  Setup framebuffer
			glCall(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);
			glCall(glViewport, 0, 0, contextConfig.width, contextConfig.height);

			//  Load texture and program
			context.BindResult();

			//  Render result
			context.RenderScreenQuad();

			// Buffer swapping
			glfwSwapBuffers(window);

			// Update time and framecount
			frameCount++;
		}
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
	code = parseOptions(shaderId, shaderApiKey, dumpShaders, argc, argv);
	if (code > 0)
		return code;

	// Context configuration
	shadertoy::ContextConfig contextConfig;
	contextConfig.width = 640;
	contextConfig.height = 480;
	contextConfig.targetFramerate = 60.0;
	contextConfig.dumpShaders = dumpShaders;

	// Fetch shader code
	code = loadRemote(shaderId, shaderApiKey, contextConfig);
	if (code > 0)
		return code;

	// Render
	code = performRender(contextConfig);

	return code;
}

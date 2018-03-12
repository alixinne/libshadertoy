#include <iostream>
#include <string>
#include <sstream>
#include <set>

#include <curl/curl.h>
#include <json/json.h>

#include <epoxy/gl.h>

#include <boost/log/trivial.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include <shadertoy.hpp>

#include "demo.h"

using namespace std;
using shadertoy::gl::gl_call;

namespace fs = boost::filesystem;

struct my_context
{
	shadertoy::render_context *context;
	shadertoy::context_config config;

	shadertoy::gl::query *fps_query;
	GLuint64 last_query_value;
	int last_query_count;

	int frame_count;
	double last_clock;
} ctx{nullptr, shadertoy::context_config(), nullptr, 0, 0, 0, 0.0};

void shadertoy_resize(int width, int height)
{
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

int load_remote(const string &shaderId, const string &shaderApiKey)
{
	CURL *curl = curl_easy_init();

	if (!curl)
	{
		BOOST_LOG_TRIVIAL(error) << "Failed to initialize curl.";
		return 1;
	}

	fs::path tmpdir(fs::temp_directory_path() / "xscreensaver-shadertoy");
	fs::file_status tmpdir_status(fs::status(tmpdir));
	if (fs::exists(tmpdir_status))
	{
		if (!fs::is_directory(tmpdir))
		{
			fs::remove(tmpdir);
			fs::create_directory(tmpdir);
		}
	}
	else
	{
		fs::create_directory(tmpdir);
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

		// Create buffer configs for each render pass
		for (int i = 0; i < shaderSpec["Shader"]["renderpass"].size(); ++i)
		{
			auto &pass(shaderSpec["Shader"]["renderpass"][i]);

			// Create buffer
			shadertoy::buffer_config imageBuffer;
			imageBuffer.name = pass["name"].asString();
			if (imageBuffer.name.empty())
				imageBuffer.name = pass["type"].asString();

			std::transform(imageBuffer.name.begin(), imageBuffer.name.end(),
					imageBuffer.name.begin(), ::tolower);

			// Skip if sound buffer
			if (pass["type"].asString().compare("sound") == 0)
			{
				BOOST_LOG_TRIVIAL(warning) << "Skipping unsupported sound shader.";
				continue;
			}

			// Load code
			stringstream sspath;
			sspath << shaderId << "-" << i << ".glsl";
			fs::path p(tmpdir / sspath.str());

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
					fs::path dstpath(tmpdir / srcpath.filename());

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
					std::transform(conf.source.begin(), conf.source.end(),
							conf.source.begin(), ::tolower);

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
			ctx.config.buffer_configs.insert(make_pair(imageBuffer.name,
														 imageBuffer));
		}

		// Image buffer should be last
		pair<string, shadertoy::buffer_config> imagebuf(*ctx.config.buffer_configs.begin());
		ctx.config.buffer_configs.erase(ctx.config.buffer_configs.begin());
		ctx.config.buffer_configs.insert(imagebuf);
	}
	catch (exception &ex)
	{
		BOOST_LOG_TRIVIAL(error) << ex.what();
		code = 1;
	}

	// Free CURL
	curl_easy_cleanup(curl);

	return code;
}

double now()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);

	return tp.tv_sec + (tp.tv_nsec / 1e9);
}

void shadertoy_render_frame()
{
	auto &state(ctx.context->state());

	// Update uniforms
	//  iTime and iFrame
	state.get<shadertoy::iFrame>() = ctx.frame_count;
	state.get<shadertoy::iTime>() = now() - ctx.last_clock;

	// No measurement of GL_TIMESTAMP yet, add it
	if (ctx.last_query_value == 0)
	{
		ctx.fps_query->query_counter(GL_TIMESTAMP);
	}

	GLint available = 0;
	ctx.fps_query->get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);

	if (available)
	{
		// The time stamp is available
		GLuint64 currentTime;
		ctx.fps_query->get_object_ui64v(GL_QUERY_RESULT, &currentTime);

		double timeDelta = (1e-9 * (currentTime - ctx.last_query_value)) / (double)(ctx.frame_count - ctx.last_query_count);

		state.get<shadertoy::iTimeDelta>() = timeDelta;
		state.get<shadertoy::iFrameRate>() = 1.0 / timeDelta;

		ctx.last_query_value = currentTime;
		ctx.last_query_count = ctx.frame_count;

		ctx.fps_query->query_counter(GL_TIMESTAMP);
	}

	//  iDate
	boost::posix_time::ptime dt = boost::posix_time::microsec_clock::local_time();
	state.get<shadertoy::iDate>() = glm::vec4(dt.date().year() - 1,
									dt.date().month(),
									dt.date().day(),
									dt.time_of_day().total_nanoseconds() / 1e9f);

	// End update uniforms

	// Render to texture
	ctx.context->render();

	// Render to screen
	//  Setup framebuffer
	gl_call(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);
	gl_call(glViewport, 0, 0, ctx.config.width, ctx.config.height);

	//  Load texture and program
	ctx.context->bind_result();

	//  Render result
	ctx.context->render_screen_quad();

	// Update framecount
	ctx.frame_count++;
}

int shadertoy_load(const char *shader_id, const char *shader_api_key, int width, int height)
{
	int code = 0;

	// Options
	string shaderId(shader_id), shaderApiKey(shader_api_key);

	ctx.config = shadertoy::context_config();

	// Context configuration
	ctx.config.width = width;
	ctx.config.height = height;
	ctx.config.target_framerate = 30.0;
	ctx.config.dump_shaders = false;

	// Fetch shader code
	code = load_remote(shaderId, shaderApiKey);
	if (code > 0)
		return code;

	// Initialize ctx
	ctx.frame_count = 0;
	ctx.last_clock = now();

	// Create context
	ctx.context = new shadertoy::render_context(ctx.config);

	try
	{
		// Initialize context
		ctx.context->init();
		BOOST_LOG_TRIVIAL(info) << "Initialized rendering context";

		auto &state(ctx.context->state());
		state.get<shadertoy::iTimeDelta>() = 1 / ctx.config.target_framerate;
		state.get<shadertoy::iFrameRate>() = ctx.config.target_framerate;

		// Create fps query
		ctx.fps_query = new shadertoy::gl::query(GL_TIMESTAMP);
		ctx.last_query_value = 0;
		ctx.last_query_count = 0;
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

	if (code != 0)
	{
		delete ctx.context;
		ctx.context = nullptr;
	}

	return code;
}

int shadertoy_init(const char *api_key, const char *query, const char *sort, int width, int height)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();

	if (!curl)
	{
		BOOST_LOG_TRIVIAL(error) << "Failed to initialize curl.";
		curl_global_cleanup();
		return 1;
	}

	// Build search query url
	std::stringstream ss;
	ss << "https://www.shadertoy.com/api/v1/shaders/query/"
		<< query
		<< "?sort="
		<< sort
		<< "&key="
		<< api_key;

	// Get returned json
	Json::Value sr = json_get(curl, ss.str());

	// Iterate shaders
	bool foundShader = false;

	srand(time(NULL) * getpid());
	std::set<std::string> testedShaders;

	for (; testedShaders.size() < sr["Results"].size(); )
	{
		std::string shaderId = sr["Results"][rand() % sr["Results"].size()].asString();

		if (testedShaders.count(shaderId) == 0)
		{
			int code = shadertoy_load(shaderId.c_str(), api_key, width, height);

			if (code == 0)
			{
				foundShader = true;
				break;
			}

			testedShaders.insert(shaderId);
		}
	}

	if (!foundShader)
	{
		int code = shadertoy_load("XsyGRW", api_key, width, height);

		if (code != 0)
		{
			abort();
		}
	}

	// Free curl memory
	curl_easy_cleanup(curl);
}

void shadertoy_free()
{
	if (ctx.fps_query)
	{
		delete ctx.fps_query;
		ctx.fps_query = nullptr;
	}

	if (ctx.context)
	{
		delete ctx.context;
		ctx.context = nullptr;
	}
}

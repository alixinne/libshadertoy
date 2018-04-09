#include <iostream>
#include <string>
#include <sstream>
#include <set>

#include <epoxy/gl.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include "api.hpp"
#include "demo.h"

using namespace std;
using shadertoy::gl::gl_call;

namespace fs = boost::filesystem;
namespace u = shadertoy::utils;

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

	// Fetch shader code
	code = load_remote(ctx.config, shaderId, shaderApiKey);
	if (code != 0)
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
		u::log::shadertoy()->info("Initialized rendering context");

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
		u::log::shadertoy()->error("Failed to initialize curl.");
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

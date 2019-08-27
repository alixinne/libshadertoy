#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include <epoxy/gl.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include "api.hpp"
#include "demo.h"

#include <shadertoy/backends/gl4.hpp>
#include <shadertoy/utils/log.hpp>

#define TEST_NO_GLFW
#include "test.hpp"

using namespace std;

namespace fs = boost::filesystem;
namespace u = shadertoy::utils;
namespace gx = shadertoy::backends::gx;
namespace gl4 = shadertoy::backends::gl4;

struct my_context : public example_ctx
{
	std::unique_ptr<gl4::query> fps_query;
	GLuint64 last_query_value;
	int last_query_count;

	int frame_count;
	double last_clock;

	my_context()
	: example_ctx(), fps_query(std::make_unique<gl4::query>(GL_TIMESTAMP)),
	  last_query_value(0), last_query_count(0), frame_count(0), last_clock(0.0)
	{
	}
};

std::unique_ptr<my_context> ctx;

void shadertoy_resize(int width, int height)
{
	// Reallocate textures
	ctx->render_size = shadertoy::rsize(width, height);
	ctx->context.allocate_textures(ctx->chain);
}

double now()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);

	return tp.tv_sec + (tp.tv_nsec / 1e9);
}

void shadertoy_render_frame()
{
	// Update uniforms
	//  iTime and iFrame
	ctx->chain.set_uniform("iFrame", ctx->frame_count);
	ctx->chain.set_uniform("iTime", static_cast<float>(now() - ctx->last_clock));

	// No measurement of GL_TIMESTAMP yet, add it
	if (ctx->last_query_value == 0)
	{
		ctx->fps_query->query_counter(GL_TIMESTAMP);
	}

	GLint available = 0;
	ctx->fps_query->get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);

	if (available)
	{
		// The time stamp is available
		GLuint64 currentTime;
		ctx->fps_query->get_object_ui64v(GL_QUERY_RESULT, &currentTime);

		float timeDelta = (1e-9 * (currentTime - ctx->last_query_value)) /
						  (double)(ctx->frame_count - ctx->last_query_count);

		ctx->chain.set_uniform("iTimeDelta", timeDelta);
		ctx->chain.set_uniform("iFrameRate", 1.0f / timeDelta);

		ctx->last_query_value = currentTime;
		ctx->last_query_count = ctx->frame_count;

		ctx->fps_query->query_counter(GL_TIMESTAMP);
	}

	//  iDate
	boost::posix_time::ptime dt = boost::posix_time::microsec_clock::local_time();
	ctx->chain.set_uniform("iDate", glm::vec4(dt.date().year() - 1, dt.date().month(), dt.date().day(),
											  dt.time_of_day().total_nanoseconds() / 1e9f));

	// End update uniforms

	// Render to texture
	ctx->context.render(ctx->chain);

	// Update framecount
	ctx->frame_count++;

	if (libshadertoy_test_exit())
		exit(0);
}

int shadertoy_load(const char *shader_id, const char *shader_api_key)
{
	int code = 0;

	// Options
	string shaderId(shader_id), shaderApiKey(shader_api_key);

	// Fetch shader code
	ctx->chain = shadertoy::swap_chain();
	code = load_remote(ctx->context, ctx->chain, ctx->render_size, shaderId, shaderApiKey);

	if (code != 0)
		return code;

	// Add screen_member
	ctx->chain.emplace_back<shadertoy::members::screen_member>(shadertoy::make_size_ref(ctx->render_size));

	try
	{
		// Initialize chain
		ctx->context.init(ctx->chain);
		u::log::shadertoy()->info("Initialized rendering swap chain");

		// Reset ctx
		ctx->frame_count = 0;
		ctx->last_clock = now();
		ctx->last_query_value = 0;
		ctx->last_query_count = 0;

		ctx->chain.set_uniform("iTimeDelta", 1.0f / 30.0f);
		ctx->chain.set_uniform("iFrameRate", 30.0f);
	}
	catch (gx::shader_compilation_error &sce)
	{
		std::cerr << "Failed to compile shader: " << sce.log();
		code = 2;
	}
	catch (shadertoy::shadertoy_error &err)
	{
		std::cerr << "Error: " << err.what();
		code = 2;
	}

	if (code != 0)
	{
		ctx->chain = shadertoy::swap_chain();
	}

	return code;
}

int shadertoy_init(const char *api_key, const char *query, const char *sort, int width, int height)
{
	shadertoy::rsize size(width, height);
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
	ss << "https://www.shadertoy.com/api/v1/shaders/query/" << query << "?sort=" << sort << "&key=" << api_key;

	// Get returned json
	Json::Value sr = json_get(curl, ss.str());

	// Set the backend to raw OpenGL 4
	shadertoy::backends::current = std::make_unique<shadertoy::backends::gl4::backend>();

	// Create context
	ctx = std::make_unique<my_context>();
	ctx->render_size = size;

	// Iterate shaders
	bool foundShader = false;

	srand(time(NULL) * getpid());
	std::set<std::string> testedShaders;

	for (; testedShaders.size() < sr["Results"].size();)
	{
		std::string shaderId = sr["Results"][rand() % sr["Results"].size()].asString();

		if (testedShaders.count(shaderId) == 0)
		{
			int code = shadertoy_load(shaderId.c_str(), api_key);

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
		int code = shadertoy_load("XsyGRW", api_key);

		if (code != 0)
		{
			abort();
		}
	}

	// Free curl memory
	curl_easy_cleanup(curl);

	return 0;
}

void shadertoy_free() { ctx.reset(); }

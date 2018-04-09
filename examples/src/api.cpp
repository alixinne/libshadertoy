#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <epoxy/gl.h>

#include "api.hpp"

using namespace std;

namespace u = shadertoy::utils;
namespace fs = boost::filesystem;

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

int load_remote(shadertoy::context_config &ctx_config, const string &shaderId, const string &shaderApiKey)
{
	CURL *curl = curl_easy_init();

	if (!curl)
	{
		u::log::shadertoy()->error("Failed to initialize curl.");
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
		u::log::shadertoy()->info("Fetching shader info from {}", endpoint);
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
				u::log::shadertoy()->warn("Skipping unsupported sound shader.");
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
						u::log::shadertoy()->info("Downloading {}", url);
						file_get(curl, url, dstpath);
					}
					else
					{
						u::log::shadertoy()->info("Using cache for {}", url);
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

					u::log::shadertoy()->debug("Pass {}, input {}: binding {} buffer {}", i, input["channel"].asInt(),
											   conf.source, conf.enabled());
				}
				else
				{
					u::log::shadertoy()->warn("Unsupported input {} for pass {}, input {}", input["ctype"].asString(),
											  i, input["channel"].asInt());
				}
			}

			// Add to context
			imageBuffer.shader_files.push_back(p);
			ctx_config.buffer_configs.emplace_back(imageBuffer.name, imageBuffer);
		}

		// Image buffer should be last
		pair<string, shadertoy::buffer_config> imagebuf(*ctx_config.buffer_configs.begin());
		ctx_config.buffer_configs.erase(ctx_config.buffer_configs.begin());
		ctx_config.buffer_configs.push_back(imagebuf);
	}
	catch (exception &ex)
	{
		u::log::shadertoy()->critical(ex.what());
		code = 1;
	}

	// Free CURL
	curl_easy_cleanup(curl);

	return code;
}


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

std::string to_buffer_name(const Json::Value &pass)
{
	auto name(pass["name"].asString());
	if (name.empty())
		name = pass["type"].asString();
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	return name;
}

void apply_sampler_options(std::shared_ptr<shadertoy::inputs::basic_input> &buffer_input, const Json::Value &sampler)
{
	if (buffer_input)
	{
		if (sampler["filter"].compare("mipmap") == 0)
		{
			buffer_input->min_filter(GL_LINEAR_MIPMAP_LINEAR);
			buffer_input->mag_filter(GL_LINEAR);
		}
		else if (sampler["filter"].compare("linear") == 0)
		{
			buffer_input->min_filter(GL_LINEAR);
			buffer_input->mag_filter(GL_LINEAR);
		}
		else if (sampler["filter"].compare("nearest") == 0)
		{
			buffer_input->min_filter(GL_LINEAR);
			buffer_input->mag_filter(GL_LINEAR);
		}

		if (sampler["wrap"].compare("repeat") == 0)
		{
			buffer_input->wrap(GL_REPEAT);
		}
		else if (sampler["wrap"].compare("clamp") == 0)
		{
			buffer_input->wrap(GL_CLAMP_TO_EDGE);
		}
	}
}

void load_nonbuffer_input(std::shared_ptr<shadertoy::inputs::basic_input> &buffer_input,
						  const Json::Value &input, CURL *curl, const fs::path &tmpdir, int i)
{
	auto &sampler(input["sampler"]);

	if (input["ctype"].compare("texture") == 0 || input["ctype"].compare("cubemap") == 0)
	{
		fs::path srcpath(input["src"].asString());
		string url = string("https://www.shadertoy.com") + input["src"].asString();
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

		shadertoy::utils::input_loader loader;

		auto uri(dstpath.string());
		std::transform(uri.begin(), uri.end(), uri.begin(), [](char cc)
			{ if (cc == '\\') return '/'; return cc; });

		buffer_input = loader.create("file:///" + uri);

		if (buffer_input)
			std::static_pointer_cast<shadertoy::inputs::file_input>(buffer_input)->vflip(sampler["vflip"].compare("true") == 0);

		apply_sampler_options(buffer_input, sampler);
	}
	else
	{
		u::log::shadertoy()->warn("Unsupported input {} for pass {}, input {}",
								  input["ctype"].asString(), i, input["channel"].asInt());
	}
}

void load_buffer_input(std::shared_ptr<shadertoy::inputs::basic_input> &buffer_input, const Json::Value &input,
					   std::map<std::string, std::shared_ptr<shadertoy::members::buffer_member>> known_buffers, int i)
{
	auto &sampler(input["sampler"]);

	if (input["ctype"].compare("buffer") == 0)
	{
		std::string source = "Buf A";
		source.back() = 'A' + (input["id"].asInt() - 257);
		std::transform(source.begin(), source.end(), source.begin(), ::tolower);

		u::log::shadertoy()->info("Pass {}, input {}: binding {} buffer", i, input["channel"].asInt(), source);

		auto src = known_buffers[source];
		assert(src);
		buffer_input = std::make_shared<shadertoy::inputs::buffer_input>(src);

		apply_sampler_options(buffer_input, sampler);
	}
}

int load_remote(shadertoy::render_context &context, shadertoy::swap_chain &chain, const shadertoy::rsize &size,
				const string &shaderId, const string &shaderApiKey)
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

		std::map<std::string, std::shared_ptr<shadertoy::members::buffer_member>> known_buffers;

		// Create buffer configs for each render pass
		for (int i = 0; i < shaderSpec["Shader"]["renderpass"].size(); ++i)
		{
			auto &pass(shaderSpec["Shader"]["renderpass"][i]);

			// Find buffer name
			auto name(to_buffer_name(pass));

			// Skip if sound buffer
			if (pass["type"].asString().compare("sound") == 0)
			{
				u::log::shadertoy()->warn("Skipping unsupported sound shader.");
				continue;
			}

			// Create buffer
			auto buffer(std::make_shared<shadertoy::buffers::toy_buffer>(name));

			// Create iChannel0-3 beforehand
			for (int j = 0; j < 4; ++j)
				buffer->inputs().emplace_back();

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

			buffer->source_files().push_back(p.string());

			// Load inputs
			for (int j = 0; j < pass["inputs"].size(); ++j)
			{
				auto &input(pass["inputs"][j]);
				auto channel_id(input["channel"].asInt());

				load_nonbuffer_input(buffer->inputs()[channel_id].input(), input, curl, tmpdir, i);
			}

			auto member(shadertoy::members::member_data(buffer, shadertoy::make_size_ref(size)));
			known_buffers.emplace(name, member);

			if (name != "image")
			{
				// Add to chain
				chain.push_back(member);
			}
		}

		// Create buffer configs for each render pass
		for (int i = 0; i < shaderSpec["Shader"]["renderpass"].size(); ++i)
		{
			auto &pass(shaderSpec["Shader"]["renderpass"][i]);

			// Find buffer name
			auto name(to_buffer_name(pass));

			// Skip if sound buffer
			if (pass["type"].asString().compare("sound") == 0)
				continue;

			// Fetch buffer
			auto buffer_member(known_buffers[name]);
			auto buffer(std::static_pointer_cast<shadertoy::buffers::toy_buffer>(buffer_member->buffer()));

			// Load buffer inputs and apply options
			for (int j = 0; j < pass["inputs"].size(); ++j)
			{
				auto &input(pass["inputs"][j]);
				auto channel_id(input["channel"].asInt());

				load_buffer_input(buffer->inputs()[channel_id].input(), input, known_buffers, i);
			}
		}

		// Add the image buffer last
		chain.push_back(known_buffers["image"]);
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


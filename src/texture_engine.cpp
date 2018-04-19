#include <map>
#include <sstream>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "shadertoy/buffer_config.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/texture_engine.hpp"

#include "shadertoy/inputs/checker_input.hpp"
#include "shadertoy/inputs/error_input.hpp"
#include "shadertoy/inputs/exr_input.hpp"
#include "shadertoy/inputs/jpeg_input.hpp"
#include "shadertoy/inputs/noise_input.hpp"
#include "shadertoy/inputs/soil_input.hpp"

using namespace std;
using namespace std::placeholders;
using namespace shadertoy;
using namespace shadertoy::utils;

namespace fs = boost::filesystem;

shared_ptr<inputs::basic_input> texture_engine::soil_input_handler(const input_config &inputConfig, bool &framebufferSized)
{
	if (inputConfig.source.empty())
	{
		log::shadertoy()->error("Missing source path for input {}", inputConfig.id);
		return shared_ptr<inputs::basic_input>();
	}

	fs::path texPath(inputConfig.source);

	if (!fs::exists(texPath))
	{
		log::shadertoy()->error("{} not found for input {}", texPath, inputConfig.id);
		return shared_ptr<inputs::basic_input>();
	}

	shared_ptr<inputs::basic_input> result_input;
	string ext(texPath.extension().string());
	boost::algorithm::to_lower(ext);

	if (ext.compare(".jpg") == 0 || ext.compare(".jpeg") == 0)
	{
		auto input(make_shared<inputs::jpeg_input>(inputConfig.source));
		input->vflip(inputConfig.vflip);
		result_input = input;
	}
	else if (ext.compare(".exr") == 0)
	{
		auto input(make_shared<inputs::exr_input>(inputConfig.source));
		input->vflip(inputConfig.vflip);
		result_input = input;
	}
	else 
	{
		auto input(make_shared<inputs::soil_input>(inputConfig.source));
		input->vflip(inputConfig.vflip);
		result_input = input;
	}

	if (result_input)
	{
		result_input->load();
		log::shadertoy()->info("Loaded '{}' for input {}", inputConfig.source, inputConfig.id);
	}

	return result_input;
}

shared_ptr<inputs::basic_input> texture_engine::noise_input_handler(const input_config &inputConfig, bool &framebufferSized)
{
	// Generate noise input
	auto input = make_shared<inputs::noise_input>(rsize_ref([this]() { return config_.render_size; }));

	input->load();
	log::shadertoy()->info("Generated noise texture for input {}", inputConfig.id);

	framebufferSized = true;

	return input;
}

shared_ptr<inputs::basic_input> texture_engine::checker_input_handler(const input_config &inputConfig, bool &framebufferSized)
{
	stringstream ss(inputConfig.source);
	int size = 0;
	ss >> size;
	if (ss.fail()) size = 10;

	// A checkerboard texture
	auto input = make_shared<inputs::checker_input>(
			rsize_ref([this]() { return config_.render_size; }),
			size);

	input->load();
	log::shadertoy()->info("Generated {}x{} checker texture for input {}", size, size, inputConfig.id);

	framebufferSized = true;

	return input;
}

texture_engine::texture_engine(context_config &config)
	: config_(config)
{
	register_handler("texture", bind(&texture_engine::soil_input_handler, this, _1, _2));
	register_handler("noise", bind(&texture_engine::noise_input_handler, this, _1, _2));
	register_handler("checker", bind(&texture_engine::checker_input_handler, this, _1, _2));
}

void texture_engine::init()
{
	if (!error_input_)
	{
		error_input_ = make_shared<inputs::error_input>();
	}
}

void texture_engine::clear(bool framebufferSizeChange)
{
	vector<string> errorTextures;

	for (auto &pair : inputs_)
	{
		if (get<0>(pair.second) == error_input_)
		{
			// If an input is in error state, reset it if this is
			// a full reset
			if (!framebufferSizeChange)
				errorTextures.push_back(pair.first);
		}
		else
		{
			// If an input is valid, reset it if it's a framebuffer
			// size change and the texture is framebuffer-sized, or
			// if it's a full reset
			if (!framebufferSizeChange || get<1>(pair.second))
				get<0>(pair.second)->reset();
		}
	}

	// Clear textures
	for (auto &obsoleteId : errorTextures)
	{
		inputs_.erase(obsoleteId);
	}
}

inputs::basic_input &texture_engine::input_texture(const input_config &inputConfig)
{
	if (!inputConfig.enabled())
	{
		return *error_input_;
	}

	auto it = inputs_.find(inputConfig.id);
	if (it == inputs_.end())
	{
		auto handlerIt = handlers_.find(inputConfig.type);
		if (handlerIt != handlers_.end())
		{
			bool framebufferSized = false;
			auto input = handlerIt->second(inputConfig, framebufferSized);

			if (input)
			{
				apply_input_options(inputConfig, *input);

				inputs_.insert(make_pair(inputConfig.id, make_tuple(input, framebufferSized)));
			}
			else
			{
				log::shadertoy()->error("Failed loading {} texture for input {}", inputConfig.type,
										inputConfig.id);

				inputs_.insert(make_pair(inputConfig.id, make_tuple(error_input_, false)));
			}
		}
		else
		{
			log::shadertoy()->error("Unknown texture type {} for input {}", inputConfig.type, inputConfig.id);
		}
	}

	return *get<0>(inputs_.find(inputConfig.id)->second);
}

void texture_engine::apply_input_options(const input_config &inputConfig, inputs::basic_input &input)
{
	input.min_filter(inputConfig.min_filter);
	input.mag_filter(inputConfig.mag_filter);
	input.wrap(inputConfig.wrap);
}

void texture_engine::register_handler(const string &name, input_handler handler)
{
	if (name.empty())
		throw std::runtime_error("Input handler name cannot be empty.");

	log::shadertoy()->debug("Registered {} texture handler", name);
	handlers_.insert(make_pair(name, handler));
}

#include <algorithm>
#include <boost/filesystem.hpp>
#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/uri.hpp"

#include "shadertoy/inputs/basic_input.hpp"

#include "shadertoy/utils/input_factories.hpp"
#include "shadertoy/utils/input_loader.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;

input_loader::input_loader()

{
	add(std::make_unique<soil_input_factory>());
	add(std::make_unique<jpeg_input_factory>());
	add(std::make_unique<exr_input_factory>());
	
	add(std::make_unique<noise_input_factory>());
	add(std::make_unique<checker_input_factory>());
}

void input_loader::add(std::unique_ptr<input_factory> &&factory)
{
	factories_.emplace(std::move(factory));
}

std::unique_ptr<inputs::basic_input> input_loader::create(const std::string &input, bool throw_on_failure) const
{
	uri url(input);
	auto spec(url.get_query_dictionary());
	spec.emplace(std::string(), url.get_path());

	for (auto &factory : factories_)
	{
		if (factory->type_name() == url.get_scheme())
		{
			if (factory->supported(spec))
			{
				return factory->create(spec);
			}
		}
	}

	if (throw_on_failure)
	{
		error_assert(false, "Could not find input factory for {}", input);
	}
	else
	{
		warn_assert(false, "Could not find input factory for {}", input);
	}
	return {};
}

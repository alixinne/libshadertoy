#include <algorithm>
#include <boost/filesystem.hpp>
#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/uri.hpp"

#include "shadertoy/utils/input_loader.hpp"

#include "shadertoy/utils/log.hpp"

#include "shadertoy/inputs/exr_input.hpp"
#include "shadertoy/inputs/jpeg_input.hpp"
#include "shadertoy/inputs/soil_input.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;

bool input_factory::operator<(const input_factory &rhs)
{
	if (this == &rhs)
		return false;
	if (rhs.priority() < priority())
		return true;
	return type_name() < rhs.type_name();
}

bool input_factory_ptr_comparator::operator()(const std::unique_ptr<input_factory> &lhs, const std::unique_ptr<input_factory> &rhs) const
{
	return *lhs < *rhs;
}

std::string file_ext(const std::string &path)
{
	boost::filesystem::path p(path);
	auto ext(p.extension().string());
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext;
}

class soil_input_factory : public input_factory
{
	const std::string type_name_;

public:
	int priority() const override { return 10; }

	bool supported(const std::string &spec) const override
	{
		auto ext(file_ext(spec));
		return inputs::soil_input::supported() &&
			(ext == ".bmp" || ext == ".png" || ext == ".jpg" || ext == ".tga" || ext == ".dds" || ext == ".psd" || ext == ".hdr");
	}

	std::shared_ptr<inputs::basic_input> create(const std::string &spec) const override
	{
		return std::make_shared<inputs::soil_input>(spec);
	}

	const std::string &type_name() const override { return type_name_; }

	soil_input_factory()
		: type_name_("file")
	{}
};

class jpeg_input_factory : public input_factory
{
	const std::string type_name_;

public:
	int priority() const override { return 45; }

	bool supported(const std::string &spec) const override
	{
		auto ext(file_ext(spec));
		return inputs::jpeg_input::supported() &&
			(ext == ".jpg" || ext == ".jpeg");
	}

	std::shared_ptr<inputs::basic_input> create(const std::string &spec) const override
	{
		return std::make_shared<inputs::jpeg_input>(spec);
	}

	const std::string &type_name() const override { return type_name_; }

	jpeg_input_factory()
		: type_name_("file")
	{}
};

class exr_input_factory : public input_factory
{
	const std::string type_name_;

public:
	int priority() const override { return 50; }

	bool supported(const std::string &spec) const override
	{
		auto ext(file_ext(spec));
		return inputs::exr_input::supported() && ext == ".exr";
	}

	std::shared_ptr<inputs::basic_input> create(const std::string &spec) const override
	{
		return std::make_shared<inputs::exr_input>(spec);
	}

	const std::string &type_name() const override { return type_name_; }

	exr_input_factory()
		: type_name_("file")
	{}
};

input_loader::input_loader()
	: factories_()
{
	add(std::make_unique<soil_input_factory>());
	add(std::make_unique<jpeg_input_factory>());
	add(std::make_unique<exr_input_factory>());
}

void input_loader::add(std::unique_ptr<input_factory> &&factory)
{
	factories_.emplace(std::move(factory));
}

std::shared_ptr<inputs::basic_input> input_loader::create(const std::string &input) const
{
	uri url(input);
	auto spec(url.get_path());

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

	log::shadertoy()->warn("Could not find input factory for {}", input);
	return {};
}

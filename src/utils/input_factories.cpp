#include <algorithm>
#include <boost/filesystem.hpp>
#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/utils/input_factories.hpp"

#include "shadertoy/inputs/exr_input.hpp"
#include "shadertoy/inputs/jpeg_input.hpp"
#include "shadertoy/inputs/soil_input.hpp"

#include "shadertoy/inputs/noise_input.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;

std::string file_ext(const std::string &path)
{
	boost::filesystem::path p(path);
	auto ext(p.extension().string());
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext;
}

bool soil_input_factory::supported(const std::string &spec) const
{
	auto ext(file_ext(spec));
	return inputs::soil_input::supported() &&
		(ext == ".bmp" || ext == ".png" || ext == ".jpg" || ext == ".tga" || ext == ".dds" || ext == ".psd" || ext == ".hdr");
}

std::shared_ptr<inputs::basic_input> soil_input_factory::create(const std::string &spec) const
{
	return std::make_shared<inputs::soil_input>(spec);
}

soil_input_factory::soil_input_factory()
	: type_name_("file")
{}

bool jpeg_input_factory::supported(const std::string &spec) const
{
	auto ext(file_ext(spec));
	return inputs::jpeg_input::supported() &&
		(ext == ".jpg" || ext == ".jpeg");
}

std::shared_ptr<inputs::basic_input> jpeg_input_factory::create(const std::string &spec) const
{
	return std::make_shared<inputs::jpeg_input>(spec);
}

jpeg_input_factory::jpeg_input_factory()
	: type_name_("file")
{}

bool exr_input_factory::supported(const std::string &spec) const
{
	auto ext(file_ext(spec));
	return inputs::exr_input::supported() && ext == ".exr";
}

std::shared_ptr<inputs::basic_input> exr_input_factory::create(const std::string &spec) const
{
	return std::make_shared<inputs::exr_input>(spec);
}

exr_input_factory::exr_input_factory()
	: type_name_("file")
{}

std::shared_ptr<inputs::basic_input> noise_input_factory::create(const std::string &spec) const
{
	return std::make_shared<inputs::noise_input>(make_size(rsize(128, 128)));
}

noise_input_factory::noise_input_factory()
	: type_name_("noise")
{}


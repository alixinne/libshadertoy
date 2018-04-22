#include <boost/filesystem.hpp>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "shadertoy/inputs/file_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;
using namespace shadertoy::utils;

namespace fs = boost::filesystem;

std::shared_ptr<gl::texture> file_input::load_image()
{
	if (!filename_.empty())
	{
		fs::path filepath(filename_);

		if (!fs::exists(filepath))
		{
			log::shadertoy()->error("{}: file not found", filename_);
		}
		else
		{
			return load_file(filename_, vflip_);
		}
	}

	return {};
}

file_input::file_input() : image_input(), filename_(), vflip_(true) {}

file_input::file_input(const std::string &filename)
: image_input(), filename_(filename), vflip_(true)
{
}

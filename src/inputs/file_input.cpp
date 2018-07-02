#include <boost/filesystem.hpp>

#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/assert.hpp"

#include "shadertoy/inputs/file_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

namespace fs = boost::filesystem;

using shadertoy::utils::error_assert;

std::unique_ptr<gl::texture> file_input::load_image()
{
	if (!filename_.empty())
	{
		fs::path filepath(filename_);

		error_assert(fs::exists(filepath),
					 "{}: file not found for input {}",
					 filename_,
					 (void*)this);

		return load_file(filename_, vflip_);
	}

	return {};
}

file_input::file_input() : image_input(), filename_(), vflip_(true) {}

file_input::file_input(const std::string &filename)
: image_input(), filename_(filename), vflip_(true)
{
}

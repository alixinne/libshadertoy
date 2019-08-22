#include "shadertoy/backends/gx/backend.hpp"

#include <utility>

#include "shadertoy/utils/assert.hpp"

#include "shadertoy/inputs/file_input.hpp"

#if __cpp_lib_filesystem >= 201703
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
#endif

using namespace shadertoy;
using namespace shadertoy::inputs;

using shadertoy::utils::error_assert;

std::unique_ptr<backends::gx::texture> file_input::load_image(GLenum &format)
{
	if (!filename_.empty())
	{
		fs::path filepath(filename_);

		error_assert(fs::exists(filepath), "{}: file not found for input {}", filename_,
					 static_cast<const void *>(this));

		return load_file(filename_, vflip_, format);
	}

	return {};
}

file_input::file_input() : vflip_(true) {}

file_input::file_input(std::string filename) : filename_(std::move(filename)), vflip_(true) {}

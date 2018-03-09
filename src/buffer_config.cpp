#include <map>

#include <boost/filesystem.hpp>

#include <epoxy/gl.h>

#include "shadertoy/buffer_config.hpp"

using namespace std;

namespace fs = boost::filesystem;

using namespace shadertoy;

bool input_config::enabled() const
{
	return !id.empty() && !type.empty();
}

input_config::input_config()
	: id(),
	  type(),
	  source(),
	  wrap(GL_REPEAT),
	  min_filter(GL_LINEAR),
	  mag_filter(GL_LINEAR)
{
}

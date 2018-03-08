#include <map>

#include <boost/filesystem.hpp>

#include <epoxy/gl.h>

#include "shadertoy/buffer_config.hpp"

using namespace std;

namespace fs = boost::filesystem;

using namespace shadertoy;

bool InputConfig::enabled() const { return !id.empty() && !type.empty(); }

InputConfig::InputConfig()
    : id(), type(), source(), wrap(GL_REPEAT), minFilter(GL_LINEAR),
      magFilter(GL_LINEAR) {}

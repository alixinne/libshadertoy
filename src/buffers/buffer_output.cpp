#include <epoxy/gl.h>

#include "shadertoy/buffers/buffer_output.hpp"

using namespace shadertoy::buffers;

buffer_output::buffer_output(std::string name, GLint location, GLenum type)
: name(name), location(location), type(type)
{
}


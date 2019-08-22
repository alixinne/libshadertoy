#include "shadertoy/backends/gx/program.hpp"

using namespace shadertoy::backends::gx;

program_link_error::program_link_error(GLuint programId, std::string log)
: shadertoy::shadertoy_error("OpenGL program linking error"), program_id_(programId), log_(std::move(log))
{
}

program_validate_error::program_validate_error(GLuint programId, std::string log)
: shadertoy::shadertoy_error("OpenGL program validation error"), program_id_(programId),
  log_(std::move(log))
{
}

attrib_location::~attrib_location() {}

uniform_location::~uniform_location() {}

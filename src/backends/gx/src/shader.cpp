#include "shadertoy/backends/gx/shader.hpp"

using namespace shadertoy::backends::gx;

shader_compilation_error::shader_compilation_error(GLuint shaderId, std::string log)
: shadertoy::shadertoy_error("OpenGL shader compilation error"), shader_id_(shaderId), log_(std::move(log))
{
}

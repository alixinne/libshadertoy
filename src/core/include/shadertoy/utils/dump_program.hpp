#ifndef _SHADERTOY_UTILS_DUMP_PROGRAM_HPP_
#define _SHADERTOY_UTILS_DUMP_PROGRAM_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{
namespace utils
{

std::vector<char> dump_program(const backends::gx::program &program);
}
} // namespace shadertoy

#endif /* _SHADERTOY_UTILS_DUMP_PROGRAM_HPP_ */

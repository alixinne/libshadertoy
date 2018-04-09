#ifndef _SHADERTOY_UTILS_LOG_HPP_
#define _SHADERTOY_UTILS_LOG_HPP_

#include "shadertoy/pre.hpp"

#undef CHAR_WIDTH
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace shadertoy
{
namespace utils
{

class log
{
	static bool initialized_;

public:
	static std::shared_ptr<spdlog::logger> shadertoy();
};

}
}

#endif /* _SHADERTOY_UTILS_LOG_HPP_ */
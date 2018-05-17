#ifndef _SHADERTOY_UTILS_LOG_HPP_
#define _SHADERTOY_UTILS_LOG_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/spdlog/spdlog.h"
#include "shadertoy/spdlog/fmt/ostr.h"

namespace shadertoy
{
namespace utils
{

/// Logging utility class for shadertoy
class shadertoy_EXPORT log
{
	static bool initialized_;

public:
	/**
	 * @brief Get the default logger instance for libshadertoy
	 *
	 * @return Pointer to the logger instance for libshadertoy
	 */
	static std::shared_ptr<spdlog::logger> shadertoy();
};

}
}

#endif /* _SHADERTOY_UTILS_LOG_HPP_ */

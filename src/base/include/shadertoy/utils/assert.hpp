#ifndef _SHADERTOY_UTILS_ASSERT_HPP_
#define _SHADERTOY_UTILS_ASSERT_HPP_

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/utils/log.hpp"

namespace shadertoy
{
namespace utils
{

template<typename... Args>
inline bool warn_assert(bool condition, const char *format, Args&&... args)
{
	if (!condition)
	{
		log::shadertoy()->warn(format, std::forward<Args>(args)...);
	}

	return condition;
}

template<typename Exception, typename... Args>
inline void throw_assert(bool condition, const char *format, Args&&... args)
{
	if (!condition)
	{
		auto str(fmt::format(format, std::forward<Args>(args)...));
		log::shadertoy()->error(str);
		throw Exception(str);
	}
}

template<typename... Args>
inline void error_assert(bool condition, const char *format, Args&&... args)
{
	throw_assert<shadertoy::shadertoy_error, Args...>(condition, format, std::forward<Args>(args)...);
}
}
}

#endif /* _SHADERTOY_UTILS_ASSERT_HPP_ */

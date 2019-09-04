#include "shadertoy/utils/log.hpp"

using namespace shadertoy::utils;
namespace spd = spdlog;

bool log::initialized_ = false;

std::shared_ptr<spd::logger> log::shadertoy()
{
	if (!initialized_)
	{
		initialized_ = true;
#if EMSCRIPTEN
		return spd::stdout_color_st("shadertoy");
#else
		return spd::stderr_color_st("shadertoy");
#endif
	}

	return spd::get("shadertoy");
}

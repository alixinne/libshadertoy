#include "shadertoy/utils/log.hpp"

using namespace shadertoy::utils;
namespace spd = spdlog;

bool log::initialized_ = false;

std::shared_ptr<spd::logger> log::shadertoy()
{
	if (!initialized_)
	{
		initialized_ = true;
		return spd::stderr_logger_st("shadertoy", true);
	}

	return spd::get("shadertoy");
}

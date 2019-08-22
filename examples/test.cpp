#include <chrono>
#include <cstdlib>

#include <epoxy/gl.h>

#include <GLFW/glfw3.h>

#include <shadertoy.hpp>

#include "test.hpp"

static bool test_initialized_ = false;
static bool autotest_mode_;
static std::chrono::time_point<std::chrono::system_clock> start_time_;

bool libshadertoy_test_exit()
{
	if (!test_initialized_)
	{
		test_initialized_ = true;
		start_time_ = std::chrono::system_clock::now();
		autotest_mode_ = std::getenv("LIBSHADERTOY_TESTING") != NULL;
	}
	else if (autotest_mode_)
	{
		return (std::chrono::system_clock::now() - start_time_ > std::chrono::seconds(5));
	}

	return false;
}

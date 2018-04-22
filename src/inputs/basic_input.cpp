#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/inputs/basic_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;

basic_input::basic_input() : loaded_(false) {}

void basic_input::load()
{
	if (!loaded_)
	{
		load_input();
		loaded_ = true;
	}
}

void basic_input::reset()
{
	if (loaded_)
	{
		reset_input();
		loaded_ = false;
	}
}

std::shared_ptr<gl::texture> basic_input::use()
{
	// Load if needed
	load();

	return use_input();
}

GLint basic_input::min_filter()
{
	GLint mf;
	sampler_.get_parameter(GL_TEXTURE_MIN_FILTER, &mf);
	return mf;
}

void basic_input::min_filter(GLint new_min_filter)
{
	sampler_.parameter(GL_TEXTURE_MIN_FILTER, new_min_filter);
}

void basic_input::mag_filter(GLint new_mag_filter)
{
	sampler_.parameter(GL_TEXTURE_MAG_FILTER, new_mag_filter);
}

void basic_input::wrap(GLint new_wrap)
{
	sampler_.parameter(GL_TEXTURE_WRAP_S, new_wrap);
	sampler_.parameter(GL_TEXTURE_WRAP_T, new_wrap);
	sampler_.parameter(GL_TEXTURE_WRAP_R, new_wrap);
}

std::shared_ptr<gl::texture> basic_input::bind(GLuint unit)
{
	sampler_.bind(unit);
	auto tex(use());
	tex->bind_unit(unit);
	return tex;
}

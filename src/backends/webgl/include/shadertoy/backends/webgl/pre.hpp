#ifndef _SHADERTOY_BACKENDS_WEBGL_PRE_HPP_
#define _SHADERTOY_BACKENDS_WEBGL_PRE_HPP_

#include "shadertoy/shadertoy_export.h"

#include "shadertoy/config.hpp"

#define SHADERTOY_BACKENDS_WEBGL_NAMESPACE_BEGIN \
	namespace shadertoy                        \
	{                                          \
	namespace backends                         \
	{                                          \
	namespace webgl                              \
	{
#define SHADERTOY_BACKENDS_WEBGL_NAMESPACE_END \
	}                                        \
	}                                        \
	}

#endif /* _SHADERTOY_BACKENDS_WEBGL_PRE_HPP_ */

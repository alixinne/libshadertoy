#ifndef _SHADERTOY_BACKENDS_GX_PRE_HPP_
#define _SHADERTOY_BACKENDS_GX_PRE_HPP_

#include "shadertoy/stbackend-gx_export.h"

#include "shadertoy/config.hpp"

#if !defined(__gl_h_) && !defined(__glext_h_) && !defined(__gl3_h_)
#include "shadertoy/backends/gx/types.hpp"
#endif

#define SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN \
	namespace shadertoy                       \
	{                                         \
	namespace backends                        \
	{                                         \
	namespace gx                              \
	{
#define SHADERTOY_BACKENDS_GX_NAMESPACE_END \
	}                                       \
	}                                       \
	}

#endif /* _SHADERTOY_BACKENDS_GX_PRE_HPP_ */

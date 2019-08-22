#ifndef _SHADERTOY_BACKENDS_GX_PRE_HPP_
#define _SHADERTOY_BACKENDS_GX_PRE_HPP_

#include "shadertoy/shadertoy_export.h"

#ifndef __gl_h
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

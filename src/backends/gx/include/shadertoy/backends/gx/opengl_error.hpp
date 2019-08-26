#ifndef _SHADERTOY_BACKENDS_GX_OPENGL_ERROR_
#define _SHADERTOY_BACKENDS_GX_OPENGL_ERROR_

#include "shadertoy/backends/gx/pre.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an OpenGL operation fails.
 */
class shadertoy_EXPORT opengl_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the opengl_error class.
	 *
	 * @param  error    OpenGL error code
	 * @param  extraMsg Extra information to include in what()
	 */
	explicit opengl_error(GLenum error, const std::string &extraMsg);
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_OPENGL_ERROR_ */

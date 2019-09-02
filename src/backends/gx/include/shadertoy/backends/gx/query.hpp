#ifndef _SHADERTOY_BACKENDS_GX_QUERY_HPP_
#define _SHADERTOY_BACKENDS_GX_QUERY_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN

/**
 * @brief Represents an OpenGL query.
 */
class stbackend_gx_EXPORT query : public resource
{
	public:
	/**
	 * @brief glBeginQuery
	 *
	 * @param target Query target
	 */
	virtual void begin(GLenum target) const = 0;

	/**
	 * @brief glEndQuery
	 *
	 * @param target Query target
	 */
	virtual void end(GLenum target) const = 0;

	/**
	 * @brief glGetQueryObjectiv
	 *
	 * @param pname  Parameter name
	 * @param params Address to result variable
	 */
	virtual void get_object_iv(GLenum pname, GLint *params) const = 0;

	/**
	 * @brief glGetQueryObjectui64v
	 *
	 * @param pname  Parameter name
	 * @param params Address to result variable
	 */
	virtual void get_object_ui64v(GLenum pname, GLuint64 *params) const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_QUERY_HPP_ */

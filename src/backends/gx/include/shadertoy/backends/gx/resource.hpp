#ifndef _SHADERTOY_BACKENDS_GX_RESOURCE_HPP_
#define _SHADERTOY_BACKENDS_GX_RESOURCE_HPP_

#include "shadertoy/backends/gx/pre.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Represents a named OpenGL resource
 */
class stbackend_gx_EXPORT resource
{
	public:
	/**
	 * @brief Destroy the object referenced by this resource.
	 *
	 * @throws opengl_error
	 */
	virtual ~resource();

	/**
	 * @brief Return true if this resource object holds a reference to a resource.
	 *
	 * @return true if this resource object holds a reference to a resource,
	 *         false otherwise.
	 */
	virtual operator bool() const = 0;

	/**
	 * @brief Return the underlying texture identifier referenced by this texture
	 * object. Throws an exception if this object does not hold a reference.
	 *
	 * @throws error_type
	 */
	virtual operator GLuint() const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_RESOURCE_HPP_ */

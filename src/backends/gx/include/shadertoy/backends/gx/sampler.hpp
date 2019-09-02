#ifndef _SHADERTOY_BACKENDS_GX_SAMPLER_HPP_
#define _SHADERTOY_BACKENDS_GX_SAMPLER_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
/**
 * @brief Represents an OpenGL sampler
 */
class stbackend_gx_EXPORT sampler : public resource
{
	public:
	/**
	 * @brief glBindSampler
	 *
	 * @param unit  Index of the texture unit to which the sampler is bound
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	virtual void bind(GLuint unit) const = 0;

	/**
	 * @brief glBindSampler
	 *
	 * Unbinds the current sampler for the unit \p unit.
	 *
	 * @param unit  Index of the texture unit to unbind the sampler from
	 *
	 * @throws opengl_error
	 */
	virtual void unbind(GLuint unit) const = 0;

	/**
	 * @brief glSamplerParameteri
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	virtual void parameter(GLenum pname, GLint param) const = 0;

	/**
	 * @brief glSamplerParameterf
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	virtual void parameter(GLenum pname, GLfloat param) const = 0;

	/**
	 * @brief glGetSamplerParameteriv
	 *
	 * @param pname  Parameter name to get
	 * @param params Returns the sampler parameters
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	virtual void get_parameter(GLenum pname, GLint *params) const = 0;

	/**
	 * @brief glGetSamplerParameterfv
	 *
	 * @param pname  Parameter name to get
	 * @param params Returns the sampler parameters
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	virtual void get_parameter(GLenum pname, GLfloat *params) const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_SAMPLER_HPP_ */

#ifndef _SHADERTOY_BACKENDS_WEBGL_SAMPLER_HPP_
#define _SHADERTOY_BACKENDS_WEBGL_SAMPLER_HPP_

#include "shadertoy/backends/webgl/pre.hpp"
#include "shadertoy/backends/webgl/resource.hpp"
#include "shadertoy/backends/gx/sampler.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_WEBGL_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an attempt is made to obtain the id of a null sampler.
 */
class shadertoy_EXPORT null_sampler_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the null_sampler_error class.
	 */
	explicit null_sampler_error();
};

/**
 * @brief Represents an OpenGL sampler
 */
class shadertoy_EXPORT sampler
: public resource<sampler, multi_allocator<&glGenSamplers, &glDeleteSamplers>, null_sampler_error, gx::sampler>
{
	public:
	sampler() : resource() {}
	sampler(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	/**
	 * @brief glBindSampler
	 *
	 * @param unit  Index of the texture unit to which the sampler is bound
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	void bind(GLuint unit) const override;

	/**
	 * @brief glBindSampler
	 *
	 * Unbinds the current sampler for the unit \p unit.
	 *
	 * @param unit  Index of the texture unit to unbind the sampler from
	 *
	 * @throws opengl_error
	 */
	void unbind(GLuint unit) const override;

	/**
	 * @brief glSamplerParameteri
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	void parameter(GLenum pname, GLint param) const override;

	/**
	 * @brief glSamplerParameterf
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	void parameter(GLenum pname, GLfloat param) const override;

	/**
	 * @brief glGetSamplerParameteriv
	 *
	 * @param pname  Parameter name to get
	 * @param params Returns the sampler parameters
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	void get_parameter(GLenum pname, GLint *params) const override;

	/**
	 * @brief glGetSamplerParameterfv
	 *
	 * @param pname  Parameter name to get
	 * @param params Returns the sampler parameters
	 *
	 * @throws opengl_error
	 * @throws null_sampler_error
	 */
	void get_parameter(GLenum pname, GLfloat *params) const override;
};
SHADERTOY_BACKENDS_WEBGL_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_WEBGL_SAMPLER_HPP_ */

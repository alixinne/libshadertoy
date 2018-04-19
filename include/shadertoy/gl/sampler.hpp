#ifndef _SHADERTOY_GL_SAMPLER_HPP_
#define _SHADERTOY_GL_SAMPLER_HPP_

#include "shadertoy/gl/resource.hpp"

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Error thrown when an attempt is made to obtain the id of a null sampler.
	 */
	class shadertoy_EXPORT null_sampler_error : public shadertoy::shadertoy_error
	{
	public:
		/**
		 * @brief Initializes a new instance of the null_sampler_error class.
		 */
		explicit null_sampler_error();
	};

	/**
	 * @brief Represents an OpenGL sampler
	 */
	class shadertoy_EXPORT sampler : public resource<
		sampler,
		multi_allocator<&glCreateSamplers, &glDeleteSamplers>,
		null_sampler_error>
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
		void bind(GLuint unit);

		/**
		 * @brief glSamplerParameteri
		 *
		 * @param pname Parameter name to set
		 * @param param Value of the parameter
		 *
		 * @throws opengl_error
		 * @throws null_sampler_error
		 */
		void parameter(GLenum pname, GLint param);

		/**
		 * @brief glSamplerParameterf
		 *
		 * @param pname Parameter name to set
		 * @param param Value of the parameter
		 *
		 * @throws opengl_error
		 * @throws null_sampler_error
		 */
		void parameter(GLenum pname, GLfloat param);

		/**
		 * @brief glGetSamplerParameteriv
		 *
		 * @param pname Parameter name to get
		 * @param param Returns the sampler parameters
		 *
		 * @throws opengl_error
		 * @throws null_sampler_error
		 */
		void get_parameter(GLenum pname, GLint *params);

		/**
		 * @brief glGetSamplerParameterfv
		 *
		 * @param pname Parameter name to get
		 * @param param Returns the sampler parameters
		 *
		 * @throws opengl_error
		 * @throws null_sampler_error
		 */
		void get_parameter(GLenum pname, GLfloat *params);
	};
}
}

#endif /* _SHADERTOY_GL_SAMPLER_HPP_ */

#ifndef _SHADERTOY_BACKENDS_WEBGL_QUERY_HPP_
#define _SHADERTOY_BACKENDS_WEBGL_QUERY_HPP_

#include "shadertoy/backends/webgl/pre.hpp"
#include "shadertoy/backends/webgl/resource.hpp"
#include "shadertoy/backends/gx/query.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_WEBGL_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an attempt is made to obtain the id of a null query
 */
class shadertoy_EXPORT null_query_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the null_query_error class.
	 */
	explicit null_query_error();
};

/**
 * @brief Represents an OpenGL query.
 */
class shadertoy_EXPORT query
: public resource<query, multi_allocator<&glGenQueries, &glDeleteQueries>, null_query_error, gx::query>
{
	public:
	/**
	 * @brief Create a new query
	 *
	 * @throws opengl_error
	 */
	query() : resource() {}
	query(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	/**
	 * @brief glBeginQuery
	 *
	 * @param target Query target
	 */
	void begin(GLenum target) const override;

	/**
	 * @brief glEndQuery
	 *
	 * @param target Query target
	 */
	void end(GLenum target) const override;

	/**
	 * @brief glGetQueryObjectiv
	 *
	 * @param pname  Parameter name
	 * @param params Address to result variable
	 */
	void get_object_iv(GLenum pname, GLint *params) const override;

	/**
	 * @brief glGetQueryObjectui64v
	 *
	 * @param pname  Parameter name
	 * @param params Address to result variable
	 */
	void get_object_ui64v(GLenum pname, GLuint64 *params) const override;
};
SHADERTOY_BACKENDS_WEBGL_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_WEBGL_QUERY_HPP_ */

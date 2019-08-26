#ifndef _SHADERTOY_BACKENDS_GL4_QUERY_HPP_
#define _SHADERTOY_BACKENDS_GL4_QUERY_HPP_

#include "shadertoy/backends/gl4/pre.hpp"
#include "shadertoy/backends/gl4/resource.hpp"
#include "shadertoy/backends/gx/query.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
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
 * @brief Implement the allocation logic for gl4::query objects
 */
class shadertoy_EXPORT query_allocator
{
	public:
	/**
	 * @brief Create a new query of the given type.
	 *
	 * @param  target Target of the query to create
	 * @return        Id of the created query
	 *
	 * @throws opengl_error
	 */
	GLuint create(GLenum target);

	/**
	 * @brief Delete the given query
	 *
	 * @param resource Id of the query to delete
	 *
	 * @throws opengl_error
	 */
	void destroy(GLuint resource);
};

/**
 * @brief Represents an OpenGL query.
 */
class shadertoy_EXPORT query : public resource<query, query_allocator, null_query_error, gx::query>
{
	public:
	query(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	/**
	 * @brief Create a new query for the given target.
	 *
	 * @param target Target of the new query.
	 *
	 * @throws opengl_error
	 */
	query(GLenum target);

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
	 * @brief glQueryCounter
	 *
	 * @param target Query target. Must be GL_TIMESTAMP
	 */
	void query_counter(GLenum target) const;

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
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_QUERY_HPP_ */

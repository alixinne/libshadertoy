#ifndef _SHADERTOY_GL_QUERY_HPP_
#define _SHADERTOY_GL_QUERY_HPP_

#include "shadertoy/gl/resource.hpp"

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Error thrown when an attempt is made to obtain the id of a null query
	 */
	class shadertoy_EXPORT null_query_error : public shadertoy::shadertoy_error
	{
	public:
          /**
           * @brief Initializes a new instance of the NullQueryError class.
           */
          explicit null_query_error();
	};

	class shadertoy_EXPORT query_allocator
	{
	public:
          /**
           * @brief Creates a new query of the given type.
           *
           * @param  target Target of the query to create
           * @return        Id of the created query
           *
           * @throws OpenGLError
           */
          GLuint create(GLenum target);

          /**
           * @brief Deletes the given query
           *
           * @param resource Id of the query to delete
           *
           * @throws OpenGLError
           */
          void destroy(GLuint resource);
	};

	/**
	 * @brief Represents an OpenGL query.
	 */
	class shadertoy_EXPORT query : public resource<query, query_allocator, null_query_error>
	{
	public:
          /**
           * @brief Creates a new query for the given target.
           *
           * @param target Target of the new query.
           *
           * @throws OpenGLError
           */
          query(GLenum target);

          /**
           * @brief glBeginQuery
           *
           * @param target Query target
           */
          void begin(GLenum target);

          /**
           * @brief glEndQuery
           *
           * @param target Query target
           */
          static void end(GLenum target);

          /**
           * @brief glQueryCounter
           *
           * @param target Query target. Must be GL_TIMESTAMP
           */
          void query_counter(GLenum target);

          /**
           * @brief glGetQueryObjectiv
           *
           * @param pname  Parameter name
           * @param params Address to result variable
           */
          void get_object_iv(GLenum pname, GLint *params);

          /**
           * @brief glGetQueryObjectui64v
           *
           * @param pname  Parameter name
           * @param params Address to result variable
           */
          void get_object_ui64v(GLenum pname, GLuint64 *params);
	};
}
}

#endif /* _SHADERTOY_GL_QUERY_HPP_ */

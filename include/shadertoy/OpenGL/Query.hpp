#ifndef _SHADERTOY_OPENGL_QUERY_HPP_
#define _SHADERTOY_OPENGL_QUERY_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * @brief Error thrown when an attempt is made to obtain the id of a null query
	 */
	class shadertoy_EXPORT NullQueryError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the NullQueryError class.
		 */
		explicit NullQueryError();
	};

	class shadertoy_EXPORT QueryAllocator
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
		GLuint Create(GLenum target);
		/**
		 * @brief Deletes the given query
		 *
		 * @param resource Id of the query to delete
		 *
		 * @throws OpenGLError
		 */
		void Delete(GLuint resource);
	};

	/**
	 * @brief Represents an OpenGL query.
	 */
	class shadertoy_EXPORT Query : public Resource<Query, QueryAllocator, NullQueryError>
	{
	public:
		/**
		 * @brief Creates a new query for the given target.
		 *
		 * @param target Target of the new query.
		 *
		 * @throws OpenGLError
		 */
		Query(GLenum target);

		/**
		 * @brief glBeginQuery
		 *
		 * @param target Query target
		 */
		void Begin(GLenum target);

		/**
		 * @brief glEndQuery
		 *
		 * @param target Query target
		 */
		static void End(GLenum target);

		/**
		 * @brief glGetQueryObjectiv
		 *
		 * @param pname  Parameter name
		 * @param params Address to result variable
		 */
		void GetObjectiv(GLenum pname, GLint *params);

		/**
		 * @brief glGetQueryObjectui64v
		 *
		 * @param pname  Parameter name
		 * @param params Address to result variable
		 */
		void GetObjectui64v(GLenum pname, GLuint64 *params);
	};
}
}

#endif /* _SHADERTOY_OPENGL_QUERY_HPP_ */

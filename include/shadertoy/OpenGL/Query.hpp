#ifndef _SHADERTOY_OPENGL_QUERY_HPP_
#define _SHADERTOY_OPENGL_QUERY_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * Error thrown when an attempt is made to obtain the id of a null query
	 */
	class shadertoy_EXPORT NullQueryError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * Initializes a new instance of the NullQueryError class.
		 */
		explicit NullQueryError();
	};

	class shadertoy_EXPORT Query : public Resource<
		Query,
		MultiAllocator<&glGenQueries, &glDeleteQueries>,
		NullQueryError>
	{
	public:
		/**
		 * glBeginQuery
		 *
		 * @param target Query target
		 */
		void Begin(GLenum target);

		/**
		 * glEndQuery
		 *
		 * @param target Query target
		 */
		static void End(GLenum target);

		/**
		 * glGetQueryObjectiv
		 *
		 * @param pname  Parameter name
		 * @param params Address to result variable
		 */
		void GetObjectiv(GLenum pname, GLint *params);

		/**
		 * glGetQueryObjectui64v
		 *
		 * @param pname  Parameter name
		 * @param params Address to result variable
		 */
		void GetObjectui64v(GLenum pname, GLuint64 *params);
	};
}
}

#endif /* _SHADERTOY_OPENGL_QUERY_HPP_ */

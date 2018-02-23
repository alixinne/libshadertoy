#ifndef _SHADERTOY_OPENGL_CALLER_HPP_
#define _SHADERTOY_OPENGL_CALLER_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * @brief Error thrown when an OpenGL operation fails.
	 */
	class shadertoy_EXPORT OpenGLError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the OpenGLError class.
		 *
		 * @param  error    OpenGL error code
		 * @param  extraMsg Extra information to include in what()
		 */
		explicit OpenGLError(GLenum error, const std::string &extraMsg);
	};

	/**
	 * @brief Throws an OpenGLError if glGetError returns non-zero
	 *
	 * @throws OpenGLError
	 */
	void shadertoy_EXPORT CheckErrors();

	/**
	 * @brief Invokes the given OpenGL function
	 *
	 * @param  function OpenGL function to invoke
	 *
	 * @throws OpenGLError
	 */
	template<typename glFunction>
	auto glCall(glFunction function)->typename
					std::enable_if<std::is_same<void, decltype(function())>::value,
					decltype(function())>::type
	{
		function();
		CheckErrors();
	}

	/**
	 * @brief Invokes the given OpenGL function
	 *
	 * @param  function OpenGL function to invoke
	 * @return          Return value of the OpenGL function
	 *
	 * @throws OpenGLError
	 */
	template<typename glFunction>
	auto glCall(glFunction function)->typename
					std::enable_if<!std::is_same<void, decltype(function())>::value,
					decltype(function())>::type
	{
		auto ret = function();
		CheckErrors();
		return ret;
	}

	/**
	 * @brief Invokes the given OpenGL function
	 *
	 * @param  function OpenGL function to invoke
	 * @param  params   Parameters to forward to the OpenGL function
	 *
	 * @throws OpenGLError
	 */
	template<typename glFunction, typename... Params>
	auto glCall(glFunction function, Params... params)->typename
					std::enable_if<std::is_same<void, decltype(function(params...))>::value,
					decltype(function(params...))>::type
	{
		function(std::forward<Params>(params)...);
		CheckErrors();
	}

	/**
	 * @brief Invokes the given OpenGL function
	 *
	 * @param  function OpenGL function to invoke
	 * @param  params   Parameters to forward to the OpenGL function
	 * @return          Return value of the OpenGL function
	 *
	 * @throws OpenGLError
	 */
	template<typename glFunction, typename... Params>
	auto glCall(glFunction function, Params... params)->typename
					std::enable_if<!std::is_same<void, decltype(function(params...))>::value,
					decltype(function(params...))>::type
	{
		auto ret = function(std::forward<Params>(params)...);
		CheckErrors();
		return ret;
	}
}
}

#endif /* _SHADERTOY_OPENGL_CALLER_HPP_ */

#ifndef _SHADERTOY_GL_CALLER_HPP_
#define _SHADERTOY_GL_CALLER_HPP_

#include "shadertoy/pre.hpp"

#include <type_traits>
#include <utility>

namespace shadertoy
{
namespace gl
{
	/// Check errors with the current context wrapper
	void check_errors();

	/**
	 * @brief Invoke the given OpenGL function
	 *
	 * @param  function OpenGL function to invoke
	 *
	 * @throws opengl_error
	 */
	template<typename glFunction>
	auto gl_call(glFunction function)->typename
					std::enable_if<std::is_same<void, decltype(function())>::value,
					decltype(function())>::type
	{
		function();
		check_errors();
	}

	/**
	 * @brief Invoke the given OpenGL function
	 *
	 * @param  function OpenGL function to invoke
	 * @return          Return value of the OpenGL function
	 *
	 * @throws opengl_error
	 */
	template<typename glFunction>
	auto gl_call(glFunction function)->typename
					std::enable_if<!std::is_same<void, decltype(function())>::value,
					decltype(function())>::type
	{
		auto ret = function();
		check_errors();
		return ret;
	}

	/**
	 * @brief Invoke the given OpenGL function
	 *
	 * @param  function OpenGL function to invoke
	 * @param  params   Parameters to forward to the OpenGL function
	 *
	 * @throws opengl_error
	 */
	template<typename glFunction, typename... Params>
	auto gl_call(glFunction function, Params... params)->typename
					std::enable_if<std::is_same<void, decltype(function(params...))>::value,
					decltype(function(params...))>::type
	{
		function(std::forward<Params>(params)...);
		check_errors();
	}

	/**
	 * @brief Invoke the given OpenGL function
	 *
	 * @param  function OpenGL function to invoke
	 * @param  params   Parameters to forward to the OpenGL function
	 * @return          Return value of the OpenGL function
	 *
	 * @throws opengl_error
	 */
	template<typename glFunction, typename... Params>
	auto gl_call(glFunction function, Params... params)->typename
					std::enable_if<!std::is_same<void, decltype(function(params...))>::value,
					decltype(function(params...))>::type
	{
		auto ret = function(std::forward<Params>(params)...);
		check_errors();
		return ret;
	}
}
}

#endif /* _SHADERTOY_GL_CALLER_HPP_ */

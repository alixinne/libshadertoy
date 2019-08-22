#ifndef _SHADERTOY_BACKENDS_GL4_CALLER_HPP_
#define _SHADERTOY_BACKENDS_GL4_CALLER_HPP_

#include "shadertoy/backends/gl4/pre.hpp"

#include <type_traits>
#include <utility>

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
/// Check errors with the current context wrapper
void check_errors();

/**
 * @brief Invoke the given OpenGL function
 *
 * @param  function OpenGL function to invoke
 *
 * @throws opengl_error
 */
template <typename glFunction>
auto gl_call(glFunction function) ->
typename std::enable_if<std::is_same<void, decltype(function())>::value, decltype(function())>::type
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
template <typename glFunction>
auto gl_call(glFunction function) ->
typename std::enable_if<!std::is_same<void, decltype(function())>::value, decltype(function())>::type
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
template <typename glFunction, typename... Params>
auto gl_call(glFunction function, Params... params) ->
typename std::enable_if<std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
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
template <typename glFunction, typename... Params>
auto gl_call(glFunction function, Params... params) ->
typename std::enable_if<!std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
{
	auto ret = function(std::forward<Params>(params)...);
	check_errors();
	return ret;
}
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_CALLER_HPP_ */

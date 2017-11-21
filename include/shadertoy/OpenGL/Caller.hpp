#ifndef _SHADERTOY_OPENGL_CALLER_HPP_
#define _SHADERTOY_OPENGL_CALLER_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class shadertoy_EXPORT OpenGLError : public shadertoy::ShadertoyError
	{
	public:
		explicit OpenGLError(GLenum error);
	};

	/**
	 * Throws an OpenGLError if glGetError returns non-zero
	 */
	void shadertoy_EXPORT CheckErrors();

	template<typename glFunction>
	auto shadertoy_EXPORT glCall(glFunction function)->typename
					std::enable_if<std::is_same<void, decltype(function())>::value,
					decltype(function())>::type
	{
		function();
		CheckErrors();
	}

	template<typename glFunction>
	auto shadertoy_EXPORT glCall(glFunction function)->typename
					std::enable_if<!std::is_same<void, decltype(function())>::value,
					decltype(function())>::type
	{
		auto ret = function();
		CheckErrors();
		return ret;
	}

	template<typename glFunction, typename... Params>
	auto shadertoy_EXPORT glCall(glFunction function, Params... params)->typename
					std::enable_if<std::is_same<void, decltype(function(params...))>::value,
					decltype(function(params...))>::type
	{
		function(std::forward<Params>(params)...);
		CheckErrors();
	}

	template<typename glFunction, typename... Params>
	auto shadertoy_EXPORT glCall(glFunction function, Params... params)->typename
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

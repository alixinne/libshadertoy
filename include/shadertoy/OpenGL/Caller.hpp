#ifndef _SHADERTOY_OPENGL_CALLER_HPP_
#define _SHADERTOY_OPENGL_CALLER_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class OpenGLError : public shadertoy::ShadertoyError
	{
	public:
		explicit OpenGLError(GLenum error);
	};

	/**
	 * Throws an OpenGLError if glGetError returns non-zero
	 */
	void CheckErrors();

	template<typename glFunction, typename... Params>
	auto glCall(glFunction function, Params... params)->typename
					std::enable_if<std::is_same<void, decltype(function(params...))>::value,
					decltype(function(params...))>::type
	{
		function(std::forward<Params>(params)...);
		CheckErrors();
	}

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

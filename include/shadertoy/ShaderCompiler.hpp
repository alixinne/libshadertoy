#ifndef _SHADERTOY_SHADER_COMPILER_HPP_
#define _SHADERTOY_SHADER_COMPILER_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief      ShaderCompiler is a class for compiling shaders while maintaining
 *             file names and line numbers in error messages returned by the
 *             OpenGL GLSL compiler.
 */
class ShaderCompiler
{
	/// List of named sources
	std::vector<std::pair<std::string, std::string>> namedSources;

public:
	/**
	 * @brief      Initialize a new instance of the ShaderCompiler class
	 */
	ShaderCompiler();

	/**
	 * @brief      Obtain a reference to the named sources for this compiler.
	 *
	 *             Each pair of this list is a tuple (name, source) where name
	 *             is the name of the shader source part, and source the actual
	 *             GLSL code. The parts will be compiled in the same order as
	 *             they are added to this vector.
	 */
	inline std::vector<std::pair<std::string, std::string>> &Sources()
	{ return namedSources; }

	/**
	 * @brief      Loads the sources in the provided shader object, and compiles
	 *             the program. Any program building errors will be rewritten so
	 *             known named sources will have their name in the output
	 *             instead of numerical source ids.
	 *
	 * @param      shader  The shader
	 */
	void Compile(oglplus::Shader &shader);
};

}

#endif /* _SHADERTOY_SHADER_COMPILER_HPP_ */

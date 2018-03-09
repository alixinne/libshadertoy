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
class shader_compiler
{
	/// List of named sources
	std::vector<std::pair<std::string, std::string>> named_sources_;

public:
	/**
	 * @brief      Initialize a new instance of the ShaderCompiler class
	 */
	shader_compiler();

	/**
	 * @brief      Obtain a reference to the named sources for this compiler.
	 *
	 *             Each pair of this list is a tuple (name, source) where name
	 *             is the name of the shader source part, and source the actual
	 *             GLSL code. The parts will be compiled in the same order as
	 *             they are added to this vector.
	 */
	inline std::vector<std::pair<std::string, std::string>> &sources()
	{ return named_sources_; }

	/**
	 * @brief      Loads the sources in the provided shader object, and compiles
	 *             the program. Any program building errors will be rewritten so
	 *             known named sources will have their name in the output
	 *             instead of numerical source ids.
	 *
	 * @param      shader  The shader
	 */
	void compile(gl::shader &shader);
};

}

#endif /* _SHADERTOY_SHADER_COMPILER_HPP_ */

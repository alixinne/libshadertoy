#ifndef _SHADERTOY_SHADER_COMPILER_HPP_
#define _SHADERTOY_SHADER_COMPILER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/backends/gx/shader.hpp"

namespace shadertoy
{

/**
 * @brief      ShaderCompiler is a class for compiling shaders while maintaining
 *             file names and line numbers in error messages returned by the
 *             OpenGL GLSL compiler.
 */
class shader_compiler
{
public:
	/**
	 * @brief      Load the sources in the provided shader object, and compiles
	 *             the program. Any program building errors will be rewritten so
	 *             known named sources will have their name in the output
	 *             instead of numerical source ids.
	 *
	 * @param      shader  The shader
	 * @param      sources Named sources to compile into the shader
	 *
	 *             Each pair of \p sources is a tuple (name, source) where name
	 *             is the name of the shader source part, and source the actual
	 *             GLSL code. The parts will be compiled in the same order as
	 *             they are added to this vector.
	 */
	static void compile(backends::gx::shader &shader, const std::vector<std::pair<std::string, std::string>> &sources);
};

}

#endif /* _SHADERTOY_SHADER_COMPILER_HPP_ */

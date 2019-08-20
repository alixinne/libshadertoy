#include <epoxy/gl.h>

#include <vector>

#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/gl/program.hpp"

#include "shadertoy/utils/dump_program.hpp"

using namespace shadertoy;

std::vector<char> utils::dump_program(const gl::program &program)
{
	// Allocate buffer
	GLint len, actLen;
	program.get(GL_PROGRAM_BINARY_LENGTH, &len);

	std::vector<char> progBinary(len);
	// Get binary
	GLenum format;
	program.get_binary(progBinary.size(), &actLen, &format, progBinary.data());

	return progBinary;
}

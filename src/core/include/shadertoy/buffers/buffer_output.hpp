#ifndef _SHADERTOY_BUFFERS_BUFFER_OUTPUT_HPP_
#define _SHADERTOY_BUFFERS_BUFFER_OUTPUT_HPP_

#include <string>

namespace shadertoy
{
namespace buffers
{

/**
 * @brief Represents a program output location to be used in a buffer
 */
struct buffer_output
{
	/// Name of the buffer output
	std::string name;

	/// Interface location of the output
	GLint location;

	/// Type of the output
	GLenum type;

	/**
	 * @brief Construct a new instance of the buffer_output class
	 *
	 * @param name     Name of the buffer output
	 * @param location Interface location of the output
	 * @param type     Type of the output
	 */
	buffer_output(std::string name, GLint location, GLenum type);
};
}
}

#endif /* _SHADERTOY_BUFFERS_BUFFER_OUTPUT_HPP_ */

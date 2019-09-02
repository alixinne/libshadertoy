#ifndef _SHADERTOY_BUFFERS_TOY_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_TOY_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/program_buffer.hpp"

#include "shadertoy/program_input.hpp"

#include <deque>

namespace shadertoy
{
namespace buffers
{

/**
 * @brief      Represents a buffer that operates on a full screen quad
 */
class shadertoy_EXPORT toy_buffer : public program_buffer
{
	protected:
	void init_geometry(const render_context &context, const io_resource &io) override;

	void render_geometry(const render_context &context, const io_resource &io) override;

	public:
#if SHADERTOY_HAS_PROGRAM_INTERFACE
	/**
	 * @brief      Initialize a new ShaderToy buffer
	 *
	 * @param[in]  id       Identifier for this buffer
	 */
	toy_buffer(const std::string &id);
#else
	/**
	 * @brief      Initialize a new ShaderToy buffer
	 *
	 * @param[in]  id       Identifier for this buffer
	 * @param[in]  outputs  Number of outputs for this buffer
	 */
	toy_buffer(const std::string &id, size_t outputs = 1);
#endif
};
} // namespace buffers
} // namespace shadertoy

#endif /* _SHADERTOY_BUFFERS_TOY_BUFFER_HPP_ */
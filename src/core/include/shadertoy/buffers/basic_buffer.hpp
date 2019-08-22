#ifndef _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/io_resource.hpp"

#include "shadertoy/members/buffer_member.hpp"

#include "shadertoy/backends/gx/query.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace shadertoy
{
namespace buffers
{

/**
 * @brief Represents a buffer in a swap chain.
 *
 * This class only contains shared behavior among all buffers: being identified
 * by an ID and spending time in the OpenGL pipeline.
 */
class shadertoy_EXPORT basic_buffer
{
	/// Buffer identifier
	const std::string id_;

	/// Query for iTimeDelta execution time
	std::unique_ptr<backends::gx::query> time_delta_query_;

	protected:
	/**
	 * @brief     Initialize a new base buffer
	 *
	 * @param[in] id        Identifier for this buffer
	 */
	basic_buffer(std::string id);

	public:
	/**
	 * @brief      Obtain the identifier of this buffer
	 *
	 * @return     String identifying this buffer
	 */
	inline const std::string &id() const { return id_; }

	/**
	 * @brief      Obtain the query object used to measure this buffer runtime
	 *
	 * @return     Reference to the query object
	 */
	inline const backends::gx::query &time_delta_query() const { return *time_delta_query_; }

	/**
	 * @brief      Obtain the duration of the last rendering of this buffer, in
	 *             nanoseconds. This method may block while waiting for the
	 *             query object to be available.
	 *
	 * @return     Number of nanoseconds elapsed during the rendering of this
	 *             buffer.
	 */
	uint64_t elapsed_time();
};
} // namespace buffers
} // namespace shadertoy

#endif /* _SHADERTOY_BUFFERS_BASIC_BUFFER_HPP_ */

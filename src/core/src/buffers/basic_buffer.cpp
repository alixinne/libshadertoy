#include "shadertoy/backends/gx/backend.hpp"

#include <utility>

#include "shadertoy/buffers/basic_buffer.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

basic_buffer::basic_buffer(std::string id)
: id_(std::move(id)), time_delta_query_(backends::current()->make_query(GL_TIME_ELAPSED))
{
}

uint64_t basic_buffer::elapsed_time()
{
	GLint available = 0;

	// Wait for result to be available
	while (available == 0)
	{
		time_delta_query_->get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
	}

	// Fetch result
	GLuint64 result;
	time_delta_query_->get_object_ui64v(GL_QUERY_RESULT, &result);

	return result;
}

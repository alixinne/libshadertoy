#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/buffers/compute_buffer.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;
using shadertoy::gl::gl_call;

using shadertoy::utils::log;

void compute_buffer::init_contents(const render_context &context, const io_resource &io)
{
	// Shader objects
	log::shadertoy()->trace("Compiling program for {} ({})", id(), static_cast<const void *>(this));
	host_.init_program(context, GL_COMPUTE_SHADER);

	// Print status
	log::shadertoy()->debug("Program {} ({}) has {} uniform inputs", id(), static_cast<const void *>(this),
							host_.program_intf().uniforms().resources().size());

}

void compute_buffer::allocate_contents(const render_context &context, const io_resource &io)
{
	// Nothing currently
}

void compute_buffer::render_contents(const render_context &context, const io_resource &io,
									 const members::buffer_member &member)
{
	// Prepare the program
	host_.prepare_render(context);

	// Try to set iTimeDelta
	if (auto time_delta_resource = host_.program_intf().uniforms().try_get("iTimeDelta"))
	{
		GLint available = 0;
		time_delta_query().get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
		if (available != 0)
		{
			// Result available, set uniform value
			GLuint64 timeDelta;
			time_delta_query().get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
			time_delta_resource->get_location(host_.program()).set_value(timeDelta / 1e9f);
		}
	}

	// Dispatch the program
	dispatch_compute();
}

void compute_buffer::dispatch_compute()
{
	gl_call(glDispatchCompute, num_groups_.x, num_groups_.y, num_groups_.z);
}

compute_buffer::compute_buffer(const std::string &id)
	: basic_buffer(id)
{
}

#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/buffers/compute_buffer.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

using shadertoy::utils::log;

void compute_buffer::dispatch_compute() { host_.program().dispatch_compute(num_groups_); }

compute_buffer::compute_buffer(const std::string &id) : basic_buffer(id) {}

void compute_buffer::init(const render_context &context)
{
	// Shader objects
	log::shadertoy()->trace("Compiling program for {} ({})", id(), static_cast<const void *>(this));
	host_.init_program(context, GL_COMPUTE_SHADER);

	// Print status
	log::shadertoy()->debug("Program {} ({}) has {} uniform inputs", id(), static_cast<const void *>(this),
							host_.program_intf().uniforms().resources().size());
}

void compute_buffer::allocate_textures(const render_context &context)
{
	// Nothing currently
}

void compute_buffer::dispatch(const render_context &context)
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
			time_delta_resource->get_location(host_.program())->set_value(timeDelta / 1e9f);
		}
	}

	// Dispatch the program
	dispatch_compute();
}

#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/buffers/program_buffer.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

using shadertoy::utils::log;

#if SHADERTOY_HAS_PROGRAM_INTERFACE
program_buffer::program_buffer(const std::string &id) : gl_buffer(id) {}
#else
program_buffer::program_buffer(const std::string &id, size_t outputs) : gl_buffer(id), output_count_(outputs) {}
#endif

void program_buffer::init(const render_context &context, const io_resource &io)
{
	// Initialize the geometry
	log::shadertoy()->trace("Loading geometry for {} ({})", id(), static_cast<const void *>(this));
	init_geometry(context, io);

	// Shader objects
	log::shadertoy()->trace("Compiling program for {} ({})", id(), static_cast<const void *>(this));
	host_.init_program(context, GL_FRAGMENT_SHADER);

#if SHADERTOY_HAS_PROGRAM_INTERFACE
	// Print status
	log::shadertoy()->debug("Program {} ({}) has {} uniform inputs", id(), static_cast<const void *>(this),
							host_.program_intf().uniforms().resources().size());
#endif
}

void program_buffer::render_gl_contents(const render_context &context, const io_resource &io, const rsize &size)
{
	// Prepare the program
	host_.prepare_render(context);

	// Set the current buffer resolution
	if (auto resolution_resource = host_.program_intf().try_get_uniform_location("iResolution"))
	{
		resolution_resource->set_value(glm::vec3(size.width, size.height, 1.f));
	}

#if SHADERTOY_HAS_TIME_QUERY
	// Try to set iTimeDelta
	if (auto time_delta_resource = host_.program_intf().try_get_uniform_location("iTimeDelta"))
	{
		GLint available = 0;
		time_delta_query().get_object_iv(GL_QUERY_RESULT_AVAILABLE, &available);
		if (available != 0)
		{
			// Result available, set uniform value
			GLuint64 timeDelta;
			time_delta_query().get_object_ui64v(GL_QUERY_RESULT, &timeDelta);
			time_delta_resource->set_value(timeDelta / 1e9f);
		}
	}
#endif

	// Render the program
	render_geometry(context, io);
}

std::optional<std::vector<buffer_output>> program_buffer::get_buffer_outputs() const
{
	std::vector<buffer_output> outputs;

#if SHADERTOY_HAS_PROGRAM_INTERFACE
	const auto &program_intf(host_.program_intf());
	outputs.reserve(program_intf.outputs().resources().size());

	for (const auto &output : program_intf.outputs().resources())
	{
		log::shadertoy()->debug("Discovered program output #{} layout(location = {}) {:#x} {}",
								outputs.size(), output.location, output.type, output.name);

		outputs.emplace_back(output.name, output.location, output.type);
	}
#else
	for (size_t i = 0; i < output_count_; ++i)
	{
		log::shadertoy()->debug("Assuming program output #{} layout(location = {})",
								outputs.size(), i);

		outputs.emplace_back(std::to_string(i), i, 0);
	}
#endif

	return outputs;
}

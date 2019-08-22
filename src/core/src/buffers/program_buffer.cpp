#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/buffers/program_buffer.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;

using shadertoy::utils::log;

program_buffer::program_buffer(const std::string &id) : gl_buffer(id) {}

void program_buffer::init(const render_context &context, const io_resource &io)
{
	// Initialize the geometry
	log::shadertoy()->trace("Loading geometry for {} ({})", id(), static_cast<const void *>(this));
	init_geometry(context, io);

	// Shader objects
	log::shadertoy()->trace("Compiling program for {} ({})", id(), static_cast<const void *>(this));
	host_.init_program(context, GL_FRAGMENT_SHADER);

	// Print status
	log::shadertoy()->debug("Program {} ({}) has {} uniform inputs", id(), static_cast<const void *>(this),
							host_.program_intf().uniforms().resources().size());
}

void program_buffer::render_gl_contents(const render_context &context, const io_resource &io, const rsize &size)
{
	// Prepare the program
	host_.prepare_render(context);

	// Set the current buffer resolution
	if (auto resolution_resource = host_.program_intf().uniforms().try_get("iResolution"))
	{
		resolution_resource->get_location(host_.program())->set_value(glm::vec3(size.width, size.height, 1.f));
	}

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

	// Render the program
	render_geometry(context, io);
}

std::optional<std::vector<buffer_output>> program_buffer::get_buffer_outputs() const
{
	const auto &program_intf(host_.program_intf());

	std::vector<buffer_output> outputs;
	outputs.reserve(program_intf.outputs().resources().size());

	for (const auto &output : program_intf.outputs().resources())
	{
		log::shadertoy()->debug("Discovered program output #{} layout(location = {}) {:#x} {}",
								outputs.size(), output.location, output.type, output.name);

		outputs.emplace_back(output.name, output.location, output.type);
	}

	return outputs;
}

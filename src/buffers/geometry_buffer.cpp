#include <epoxy/gl.h>

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/error_input.hpp"

#include "shadertoy/buffers/geometry_buffer.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/geometry/basic_geometry.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy;
using namespace shadertoy::buffers;
using shadertoy::utils::error_assert;

geometry_buffer::geometry_buffer(const std::string &id)
	: program_buffer(id)
{
}

void geometry_buffer::init_geometry(const render_context &context, const io_resource &io)
{
	// We assume the geometry is initialized by the caller
}

void geometry_buffer::render_geometry(const render_context &context, const io_resource &io)
{
	// Render the geometry
	auto ptr = geometry_.lock();
	error_assert(ptr != nullptr, "Failed to get the geometry to render for geometry_buffer {} ({})",
				 id(), static_cast<const void *>(this));

	ptr->render(time_delta_query());
}


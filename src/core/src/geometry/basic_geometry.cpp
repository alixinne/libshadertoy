#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/geometry/basic_geometry.hpp"

using namespace shadertoy;
using namespace shadertoy::geometry;

void basic_geometry::render() const
{
	// Draw geometry
	draw();
}

void basic_geometry::render(const backends::gx::query &timer_query) const
{
#if SHADERTOY_HAS_TIME_QUERY
	timer_query.begin(GL_TIME_ELAPSED);
#endif

	// Draw geometry
	draw();

#if SHADERTOY_HAS_TIME_QUERY
	timer_query.end(GL_TIME_ELAPSED);
#endif
}

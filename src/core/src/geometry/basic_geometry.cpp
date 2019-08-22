#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/geometry/basic_geometry.hpp"

using namespace shadertoy;
using namespace shadertoy::geometry;

void basic_geometry::render() const
{
	// Bind VAO
	const auto &vao(vertex_array());
	vao.bind();

	// Draw geometry
	draw();

	vao.unbind();
}

void basic_geometry::render(const backends::gx::query &timer_query) const
{
	// Bind VAO
	const auto &vao(vertex_array());
	vao.bind();

	timer_query.begin(GL_TIME_ELAPSED);

	// Draw geometry
	draw();

	timer_query.end(GL_TIME_ELAPSED);

	vao.unbind();
}

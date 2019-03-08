#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/geometry/basic_geometry.hpp"

using namespace shadertoy;
using namespace shadertoy::geometry;

void basic_geometry::render() const
{
	// Bind VAO
	const auto &vao(vertex_array());
	auto vao_bind(gl::get_bind_guard(vao));

	// Draw geometry
	draw();
}

void basic_geometry::render(const gl::query &timer_query) const
{
	// Bind VAO
	const auto &vao(vertex_array());
	auto vao_bind(gl::get_bind_guard(vao));

	timer_query.begin(GL_TIME_ELAPSED);

	// Draw geometry
	draw();

	gl::query::end(GL_TIME_ELAPSED);
}

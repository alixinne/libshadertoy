#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/backends/gx/program.hpp"

#include "shadertoy/geometry/screen_quad.hpp"

using namespace shadertoy;
using namespace shadertoy::geometry;

screen_quad::screen_quad()
: quad_array_(backends::current->make_vertex_array()),
  quad_corners_(backends::current->make_buffer()), quad_indices_(backends::current->make_buffer())
{
	// clang-format off
	GLfloat coords[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	// clang-format on

	// Bind VAO
	quad_array_->bind();

	// Setup coords
	quad_corners_->data(sizeof(coords), static_cast<const GLvoid *>(&coords[0]), GL_STATIC_DRAW);

	// Setup indices
	quad_indices_->data(sizeof(indices), static_cast<const GLvoid *>(&indices[0]), GL_STATIC_DRAW);

	// Setup position and texCoord attributes for shaders
	quad_corners_->bind(GL_ARRAY_BUFFER);
	quad_indices_->bind(GL_ELEMENT_ARRAY_BUFFER);

	// bind input "position" to vertex locations (3 floats)
	auto position(backends::current->make_attrib_location(0));
	position->vertex_pointer(3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), static_cast<const void *>(nullptr));
	position->enable_vertex_array();

	// bind input "texCoord" to vertex texture coordinates (2 floats)
	auto texCoord(backends::current->make_attrib_location(1));
	texCoord->vertex_pointer(2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
							 reinterpret_cast<const void *>(3 * sizeof(GLfloat))); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
	texCoord->enable_vertex_array();

	// Unbind
	quad_array_->unbind();
	quad_indices_->unbind(GL_ELEMENT_ARRAY_BUFFER);
	quad_corners_->unbind(GL_ARRAY_BUFFER);
}

void screen_quad::draw() const
{
	quad_array_->draw_elements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);
}

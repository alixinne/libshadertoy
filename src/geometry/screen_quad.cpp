#include <epoxy/gl.h>

#include "shadertoy/gl/program.hpp"

#include "shadertoy/geometry/screen_quad.hpp"

using namespace shadertoy;
using namespace shadertoy::geometry;
using shadertoy::gl::gl_call;

screen_quad::screen_quad()
{
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

	// Bind VAO
	quad_array_.bind();

	// Setup coords
	quad_corners_.data(sizeof(coords), static_cast<const GLvoid *>(&coords[0]), GL_STATIC_DRAW);

	// Setup indices
	quad_indices_.data(sizeof(indices), static_cast<const GLvoid *>(&indices[0]), GL_STATIC_DRAW);

	// Setup position and texCoord attributes for shaders
	quad_corners_.bind(GL_ARRAY_BUFFER);
	quad_indices_.bind(GL_ELEMENT_ARRAY_BUFFER);

	// bind input "position" to vertex locations (3 floats)
	gl::attrib_location position(0);
	position.vertex_pointer(3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), static_cast<const void *>(nullptr));
	position.enable_vertex_array();

	// bind input "texCoord" to vertex texture coordinates (2 floats)
	gl::attrib_location texCoord(1);
	texCoord.vertex_pointer(2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
							reinterpret_cast<const void *>(3 * sizeof(GLfloat))); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
	texCoord.enable_vertex_array();

	// Unbind
	quad_array_.unbind();
	quad_indices_.unbind(GL_ELEMENT_ARRAY_BUFFER);
	quad_corners_.unbind(GL_ARRAY_BUFFER);
}

void screen_quad::draw() const
{
	gl_call(glDrawElements, GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);
}

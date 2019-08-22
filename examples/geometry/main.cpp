#include <epoxy/gl.h>

#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>
#include <iostream>

#include <shadertoy.hpp>
#include <shadertoy/backends/gl4.hpp>
#include <shadertoy/utils/log.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "test.hpp"

#include "tiny_obj_loader.h"

namespace fs = boost::filesystem;
using namespace shadertoy;
using namespace shadertoy::backends;

// Type that will manage the geometry
class tiny_geometry : public geometry::basic_geometry
{
	/// Vertex array object
	std::unique_ptr<gx::vertex_array> vao_;
	/// Buffer objects
	std::unique_ptr<gx::buffer> vertices_;
	std::unique_ptr<gx::buffer> indices_;
	size_t indices_size_;

	public:
	tiny_geometry(const std::string &geometry_path) : basic_geometry()
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, geometry_path.c_str());

		if (!err.empty())
		{
			std::cerr << err << std::endl;
		}

		assert(ret);
		assert(shapes.size() > 0);

		const auto &mesh(shapes[0].mesh);
		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		for (const auto &index : mesh.indices)
		{
			vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
			vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
			vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

			vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
			vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
			vertices.push_back(attrib.normals[3 * index.normal_index + 2]);

			vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
			vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);

			indices.push_back(indices.size());
		}

		indices_size_ = indices.size();

		vao_ = current->make_vertex_array();
		vao_->bind();
		vertices_ = current->make_buffer();
		vertices_->bind(GL_ARRAY_BUFFER);
		indices_ = current->make_buffer();
		indices_->bind(GL_ELEMENT_ARRAY_BUFFER);

		vertices_->data(sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		indices_->data(sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

		// bind input "position" to vertex locations (3 floats)
		auto position(current->make_attrib_location(0));
		position->vertex_pointer(3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
		position->enable_vertex_array();

		// bind input "texCoord" to vertex texture coordinates (2 floats)
		auto texCoord(current->make_attrib_location(1));
		texCoord->vertex_pointer(2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		texCoord->enable_vertex_array();

		// bind input "normals" to vertex normals (3 floats)
		auto normals(current->make_attrib_location(2));
		normals->vertex_pointer(3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
		normals->enable_vertex_array();

		// Unbind
		vao_->unbind();
		indices_->unbind(GL_ELEMENT_ARRAY_BUFFER);
		vertices_->unbind(GL_ARRAY_BUFFER);
	}

	const gx::vertex_array &vertex_array() const { return *vao_; }

	void draw() const final
	{
		vao_->draw_elements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, nullptr);
	}
};

int main(int argc, char *argv[])
{
	int code = 0;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 2;
	}

	glfwSetErrorCallback([](int error, const char *description) {
		std::cerr << "GLFW error: " << description << std::endl;
	});

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Initialize window
	int width = 640, height = 480;
	GLFWwindow *window = glfwCreateWindow(width, height, "libshadertoy example - geometry", nullptr, nullptr);

	if (!window)
	{
		std::cerr << "Failed to create glfw window" << std::endl;
		code = 1;
	}
	else
	{
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		// Set the backend to raw OpenGL 4
		shadertoy::backends::current = std::make_unique<shadertoy::backends::gl4::backend>();

		utils::log::shadertoy()->set_level(spdlog::level::trace);

		try
		{
			example_ctx ctx;
			auto &context(ctx.context);
			auto &chain(ctx.chain);

			// The default vertex shader is not sufficient, we replace it with our own
			context.buffer_template()[GL_VERTEX_SHADER] =
			compiler::shader_template::parse_file(ST_BASE_DIR "/geometry/vertex.glsl");

			// Force compilation of new template
			context.buffer_template().compile(GL_VERTEX_SHADER);

			// Load geometry
			auto geometry(std::make_shared<tiny_geometry>(ST_BASE_DIR "/geometry/bunny.obj"));

			// Set the context parameters (render size and some uniforms)
			ctx.render_size = rsize(width, height);

			// Create the image buffer
			auto imageBuffer(std::make_shared<buffers::geometry_buffer>("image"));
			imageBuffer->source_file(ST_BASE_DIR "/gradient.glsl");
			imageBuffer->geometry(geometry);

			// Add the image buffer to the swap chain, at the given size
			// The default_framebuffer policy makes this buffer draw directly to
			// the window instead of using a texture that is then copied to the
			// screen.
			auto image_member = chain.emplace_back(imageBuffer, make_size_ref(ctx.render_size),
												   member_swap_policy::default_framebuffer);

			// Without a background, the buffer should also clear the previous contents
			image_member->state().clear_color({ .5f, .0f, .5f, 1.f });
			image_member->state().clear_depth(1.f);
			image_member->state().clear_bits(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Also we need depth test
			image_member->state().enable(GL_DEPTH_TEST);

			// Initialize context
			context.init(chain);
			std::cout << "Initialized swap chain" << std::endl;

			// Set uniforms
			chain.set_uniform("iTimeDelta", 1.0f / 60.0f);
			chain.set_uniform("iFrameRate", 60.0f);

			// MVP matrix location
			auto mvp_location(imageBuffer->interface().get_uniform_location("eMVP"));

			// Now render for 5s
			int frameCount = 0;
			float t = 0.;

			// Set the resize callback
			glfwSetWindowUserPointer(window, &ctx);
			glfwSetFramebufferSizeCallback(window, example_set_framebuffer_size<example_ctx>);

			while (!glfwWindowShouldClose(window))
			{
				// Poll events
				glfwPollEvents();

				// Update uniforms
				chain.set_uniform("iTime", t);
				chain.set_uniform("iFrame", frameCount);

				// Set viewport
				// This is not necessary when the last pass is rendering to a
				// texture and it is followed by a screen_member, which calls
				// glViewport. In this example, we render directly to the
				// default framebuffer, so we need to set the viewport
				// ourselves.
				shadertoy::backends::current->set_viewport(0, 0, ctx.render_size.width,
														   ctx.render_size.height);

				// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
				glm::mat4 Projection =
				glm::perspective(glm::radians(25.0f),
								 (float)ctx.render_size.width / (float)ctx.render_size.height, 0.1f, 100.0f);

				// Camera matrix
				glm::mat4 View = glm::lookAt(glm::vec3(3, 3, 3), // Camera is at (4,3,3), in World Space
											 glm::vec3(0, .75, 0), // and looks at the origin
											 glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
				);

				// Model matrix : an identity matrix (model will be at the origin)
				glm::mat4 Model =
				glm::rotate(glm::mat4(1.f), frameCount * 0.0125f, glm::vec3(0.f, 1.f, 0.f));
				// Our ModelViewProjection : multiplication of our 3 matrices
				mvp_location->set_value(Projection * View * Model);

				// Render the swap chain
				context.render(chain);

				// Buffer swapping
				glfwSwapBuffers(window);

				// Update time and framecount
				t = glfwGetTime();
				frameCount++;

				if (libshadertoy_test_exit())
					glfwSetWindowShouldClose(window, true);
			}
		}
		catch (gx::shader_compilation_error &sce)
		{
			std::cerr << "Failed to compile shader: " << sce.log();
			code = 2;
		}
		catch (shadertoy_error &err)
		{
			std::cerr << "Error: " << err.what();
			code = 2;
		}

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}

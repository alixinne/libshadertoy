#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <oglplus/all.hpp>
#include <boost/filesystem.hpp>

#include <shadertoy/Shadertoy.hpp>

namespace fs = boost::filesystem;

int main(int argc, char *argv[])
{
	int code = 0;

	shadertoy::ContextConfig contextConfig;
	contextConfig.width = 640;
	contextConfig.height = 480;
	contextConfig.targetFramerate = 60.0;

	shadertoy::BufferConfig imageBuffer;
	imageBuffer.name = "image";
	imageBuffer.shaderFiles.push_back(fs::path("..") / fs::path("shader.glsl"));

	contextConfig.bufferConfigs.insert(make_pair(imageBuffer.name,
												 imageBuffer));

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 1;
	}

	// Initialize window
	GLFWwindow *window = glfwCreateWindow(contextConfig.width,
										  contextConfig.height,
										  "libshadertoy example 10-gradient",
										  nullptr,
										  nullptr);

	if (!window)
	{
		std::cerr << "Failed to create glfw window" << std::endl;
		code = 1;
	}
	else
	{
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		// Initialize GLEW
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "Failed to initialize glew" << std::endl;
			code = 1;
		}
		else
		{
			shadertoy::RenderContext context(contextConfig);
			std::cout << "Created context based on config" << std::endl;

			// Initialize context
			context.Initialize();
			std::cout << "Initialized rendering context" << std::endl;

			// Now render for 5s
			int frameCount = 0;
			double t = 0.;

			oglplus::Context gl;
			oglplus::DefaultFramebuffer dfb;

			while (t < 5.)
			{
				// Update uniforms
				context.GetState().V<shadertoy::iGlobalTime>() = t;
				context.GetState().V<shadertoy::iFrame>() = frameCount;

				// Render to texture
				context.Render();

				// Render to screen
				//  Setup framebuffer
				dfb.Bind(oglplus::Framebuffer::Target::Draw);
				gl.Viewport(0, 0, contextConfig.width, contextConfig.height);

				//  Load texture and program
				context.BindResult();

				//  Render result
				context.RenderScreenQuad();

				// Buffer swapping
				glfwSwapBuffers(window);

				// Update time and framecount
				t = glfwGetTime();
				frameCount++;
			}
		}

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}
#include <iostream>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include <shadertoy.hpp>

int main(int argc, char *argv[])
{
	int code = 0;

	shadertoy::context_config contextConfig;
	contextConfig.width = 640;
	contextConfig.height = 480;
	contextConfig.target_framerate = 60.0;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 1;
	}

	// Initialize window
	GLFWwindow *window = glfwCreateWindow(contextConfig.width,
										  contextConfig.height,
										  "libshadertoy example 00-build",
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

		{
			shadertoy::render_context context(contextConfig);
			std::cout << "Created context based on config" << std::endl;
		}

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}
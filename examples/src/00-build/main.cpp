#include <iostream>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include <shadertoy.hpp>

int main(int argc, char *argv[])
{
	int code = 0;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize glfw" << std::endl;
		return 2;
	}

	// Initialize window
	GLFWwindow *window = glfwCreateWindow(640, 480, "libshadertoy example 00-build", nullptr, nullptr);

	if (!window)
	{
		std::cerr << "Failed to create glfw window" << std::endl;
		code = 1;
	}
	else
	{
		glfwMakeContextCurrent(window);

		{
			shadertoy::render_context context;
			std::cout << "Created context" << std::endl;
		}

		glfwDestroyWindow(window);
	}

	glfwTerminate();
	return code;
}

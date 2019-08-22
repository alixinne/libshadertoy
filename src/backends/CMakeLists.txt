add_subdirectory(gx)

option(SHADERTOY_BUILD_GL4_BACKEND "Build the OpenGL 4 raw backend" ON)
if (SHADERTOY_BUILD_GL4_BACKEND)
	add_subdirectory(gl4)
endif()

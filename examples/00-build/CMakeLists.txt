cmake_minimum_required(VERSION 2.8)

project(00-build)
include(FindPkgConfig)

# GL libraries
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
pkg_search_module(glfw3 glfw3)

# libshadertoy
find_package(shadertoy REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${glfw3_INCLUDE_DIRS}
	${shadertoy_INCLUDE_DIRS})

add_executable(example00-build
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(example00-build
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${glfw3_LIBRARIES}
	${shadertoy_LIBRARIES})

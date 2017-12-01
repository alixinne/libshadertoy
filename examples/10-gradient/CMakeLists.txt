cmake_minimum_required(VERSION 2.8)

project(10-gradient)
include(FindPkgConfig)

# GL libraries
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
pkg_search_module(glfw3 REQUIRED glfw3)

# libshadertoy
find_package(shadertoy REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${glfw3_INCLUDE_DIRS})

add_executable(example10-gradient
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(example10-gradient
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${glfw3_LIBRARIES}
	shadertoy)

# C++14
set_property(TARGET example10-gradient PROPERTY CXX_STANDARD 14)

cmake_minimum_required(VERSION 2.8)

project(00-build)
include(FindPkgConfig)

# GL libraries
find_package(OpenGL REQUIRED)
pkg_search_module(glfw3 REQUIRED glfw3)
pkg_search_module(EPOXY REQUIRED epoxy)

# libshadertoy
find_package(shadertoy REQUIRED 1.0.0)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${glfw3_INCLUDE_DIRS})

add_executable(example00-build
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(example00-build
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${glfw3_LIBRARIES}
	shadertoy-shared)

# C++14
set_property(TARGET example00-build PROPERTY CXX_STANDARD 14)

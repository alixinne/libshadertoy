cmake_minimum_required(VERSION 2.8)

project(00-build)
include(FindPkgConfig)

# GL libraries
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
pkg_search_module(glfw3 REQUIRED glfw3)

# oglplus config
set(oglplus_CONFIGURE_EXTRA_ARGS --use-glew --use-glfw3)
set(shadertoy_LOAD_OGLPLUS ON)

# libshadertoy
find_package(shadertoy REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${glfw3_INCLUDE_DIRS}
	${oglplus_INCLUDE_DIRS}
	${shadertoy_INCLUDE_DIRS})

add_executable(example00-build
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(example00-build
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${glfw3_LIBRARIES}
	${shadertoy_LIBRARIES})

# C++14
set_property(TARGET example00-build PROPERTY CXX_STANDARD 14)

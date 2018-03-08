cmake_minimum_required(VERSION 2.8)

project(50-shadertoy)
include(FindPkgConfig)

# GL libraries
find_package(OpenGL REQUIRED)
find_package(Boost 1.58 REQUIRED COMPONENTS filesystem log serialization
	date_time program_options)
find_package(CURL REQUIRED)
pkg_search_module(glfw3 REQUIRED glfw3)
pkg_search_module(jsoncpp REQUIRED jsoncpp)
pkg_search_module(EPOXY REQUIRED epoxy)

# libshadertoy
find_package(shadertoy REQUIRED 1.0.0)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${glfw3_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
	${jsoncpp_INCLUDE_DIRS})

add_executable(example50-shadertoy
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(example50-shadertoy
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${glfw3_LIBRARIES}
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	${jsoncpp_LIBRARIES}
	shadertoy)

# C++14
set_property(TARGET example50-shadertoy PROPERTY CXX_STANDARD 14)

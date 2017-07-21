cmake_minimum_required(VERSION 2.8)

project(50-shadertoy)
include(FindPkgConfig)

# GL libraries
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost 1.58 REQUIRED COMPONENTS filesystem log serialization
	date_time program_options)
find_package(CURL REQUIRED)
pkg_search_module(glfw3 REQUIRED glfw3)
pkg_search_module(jsoncpp REQUIRED jsoncpp)

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
	${Boost_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
	${jsoncpp_INCLUDE_DIRS}
	${shadertoy_INCLUDE_DIRS})

add_executable(example50-shadertoy
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(example50-shadertoy
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${glfw3_LIBRARIES}
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	${jsoncpp_LIBRARIES}
	${shadertoy_LIBRARIES})

# C++14
set_property(TARGET example50-shadertoy PROPERTY CXX_STANDARD 14)

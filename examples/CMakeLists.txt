cmake_minimum_required(VERSION 2.8)

project(libshadertoy-examples)
include(FindPkgConfig)

# OpenGL libraries
find_package(OpenGL REQUIRED)

pkg_search_module(EPOXY REQUIRED epoxy)
link_directories(${EPOXY_LIBDIR})

pkg_search_module(GLFW3 glfw3)
if (GLFW3_FOUND)
	link_directories(${GLFW3_LIBDIR})
endif()

# Extra libraries for shadertoy API examples
find_package(Boost 1.58 COMPONENTS filesystem log serialization date_time program_options)

find_package(CURL)

pkg_search_module(JSONCPP jsoncpp)
if (JSONCPP_FOUND)
	link_directories(${JSONCPP_LIBDIR})
endif()

# Extra libraries for 60-xscreensaver
find_package(X11)

# libshadertoy
find_package(shadertoy REQUIRED 1.0.0)

# Directories
set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (OpenGL_FOUND AND EPOXY_FOUND AND GLFW3_FOUND)
	add_subdirectory(src/00-build)
	add_subdirectory(src/10-gradient)
	add_subdirectory(src/15-uniforms)
else()
	message(STATUS "Not building examples 00-build, 10-gradient and 15-uniforms")
	message(STATUS "You might want to install libgl-mesa-dev, libepoxy-dev and libglfw-dev")
endif()

if (OpenGL_FOUND AND EPOXY_FOUND AND GLFW3_FOUND AND Boost_FOUND AND CURL_FOUND AND JSONCPP_FOUND)
	add_subdirectory(src/50-shadertoy)
else()
	message(STATUS "Not building example 50-shadertoy")
	message(STATUS "You also need libcurl-dev, libboost-all-dev and libjsoncpp-dev")
endif()

if (OpenGL_FOUND AND EPOXY_FOUND AND Boost_FOUND AND CURL_FOUND AND JSONCPP_FOUND AND X11_FOUND)
	add_subdirectory(3rdparty/xscreensaver)
	add_subdirectory(src/60-xscreensaver)
endif()


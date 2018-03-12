cmake_minimum_required(VERSION 2.8)

project(60-xscreensaver)
include(FindPkgConfig)

# GL libraries
find_package(OpenGL REQUIRED)
find_package(Boost 1.58 REQUIRED COMPONENTS filesystem log serialization date_time)
find_package(CURL REQUIRED)
pkg_search_module(jsoncpp REQUIRED jsoncpp)
pkg_search_module(EPOXY REQUIRED epoxy)
find_package(X11 REQUIRED)

# libshadertoy
find_package(shadertoy REQUIRED 1.0.0)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/xscreensaver
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
	${jsoncpp_INCLUDE_DIRS})

file(GLOB XSCREENSAVER_H xscreensaver/*.h)
file(GLOB XSCREENSAVER_C xscreensaver/*.c)
add_library(xscreensaver-support ${XSCREENSAVER_H} ${XSCREENSAVER_C})

target_compile_definitions(xscreensaver-support PUBLIC
	GETTIMEOFDAY_TWO_ARGS=1
	USE_GL=1
	HAVE_GL=1
	MESA=1)

target_link_libraries(xscreensaver-support PUBLIC
	${X11_LIBRARIES}
	${X11_Xft_LIB}
	${X11_Xt_LIB}
	${X11_Xmu_LIB})

add_executable(example60-xscreensaver
	${CMAKE_CURRENT_SOURCE_DIR}/demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.c)

target_link_libraries(example60-xscreensaver
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	${jsoncpp_LIBRARIES}
	shadertoy-static
	xscreensaver-support)

target_compile_definitions(example60-xscreensaver PUBLIC
	USE_GL=1
	BOOST_LOG_DYN_LINK=1)

# C++14
set_property(TARGET example60-xscreensaver PROPERTY CXX_STANDARD 14)

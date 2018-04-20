message(STATUS "Building example 60-xscreensaver")

add_executable(example60-xscreensaver
	${CMAKE_CURRENT_SOURCE_DIR}/demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
	${SRC_ROOT}/api.cpp
	${SRC_ROOT}/test.cpp)

target_include_directories(example60-xscreensaver PRIVATE
	${INCLUDE_ROOT}/60-xscreensaver
	${INCLUDE_ROOT}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
	${JSONCPP_INCLUDE_DIRS})

target_link_libraries(example60-xscreensaver
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	${JSONCPP_LIBRARIES}
	shadertoy-static
	xscreensaver-support)

target_compile_definitions(example60-xscreensaver PUBLIC USE_GL=1)

# C++14
set_property(TARGET example60-xscreensaver PROPERTY CXX_STANDARD 14)

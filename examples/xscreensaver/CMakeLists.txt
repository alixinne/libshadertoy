message(STATUS "Building example xscreensaver")

add_executable(example_xscreensaver
	${CMAKE_CURRENT_SOURCE_DIR}/demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
	${SRC_ROOT}/api.cpp
	${SRC_ROOT}/test.cpp)

target_include_directories(example_xscreensaver PRIVATE
	${ST_INC_DIR}
	${INCLUDE_ROOT}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
	${JSONCPP_INCLUDE_DIRS})

target_link_libraries(example_xscreensaver
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	${JSONCPP_LIBRARIES}
	shadertoy-static
	stbackend-gl4-static
	xscreensaver-support)

target_compile_definitions(example_xscreensaver PUBLIC USE_GL=1)

# C++17
set_property(TARGET example_xscreensaver PROPERTY CXX_STANDARD 17)

set_target_properties(example_xscreensaver PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

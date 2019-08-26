message(STATUS "Building example shadertoy")

add_executable(example_shadertoy
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${SRC_ROOT}/api.cpp
	${SRC_ROOT}/test.cpp)

target_include_directories(example_shadertoy PRIVATE
	${ST_INC_DIR}
	${INCLUDE_ROOT}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
	${JSONCPP_INCLUDE_DIRS})

target_link_libraries(example_shadertoy
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${GLFW3_LIBRARIES}
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	${JSONCPP_LIBRARIES}
	${SHADERTOY_LIBRARY}
	${SHADERTOY_GL4_BACKEND})

# C++17
set_property(TARGET example_shadertoy PROPERTY CXX_STANDARD 17)

set_target_properties(example_shadertoy PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

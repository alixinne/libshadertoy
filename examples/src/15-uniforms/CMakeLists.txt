message(STATUS "Building example 15-uniforms")

add_executable(example15-uniforms
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/uniforms.cpp
	${SRC_ROOT}/test.cpp)

target_include_directories(example15-uniforms PRIVATE
	${ST_INC_DIR}
	${INCLUDE_ROOT}
	${INCLUDE_ROOT}/15-uniforms
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS})

target_link_libraries(example15-uniforms
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${Boost_LIBRARIES}
	${GLFW3_LIBRARIES}
	shadertoy-shared)

# C++17
set_property(TARGET example15-uniforms PROPERTY CXX_STANDARD 17)

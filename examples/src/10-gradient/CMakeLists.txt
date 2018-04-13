message(STATUS "Building example 10-gradient")

add_executable(example10-gradient
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${SRC_ROOT}/test.cpp)

target_include_directories(example10-gradient PRIVATE
	${INCLUDE_ROOT}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS})

target_link_libraries(example10-gradient
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${GLFW3_LIBRARIES}
	shadertoy-shared)

# C++14
set_property(TARGET example10-gradient PROPERTY CXX_STANDARD 14)

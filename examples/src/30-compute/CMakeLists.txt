message(STATUS "Building example 30-compute")

add_executable(example30-compute
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${SRC_ROOT}/test.cpp)

target_include_directories(example30-compute PRIVATE
	${ST_INC_DIR}
	${INCLUDE_ROOT}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS})

target_link_libraries(example30-compute
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${Boost_LIBRARIES}
	${GLFW3_LIBRARIES}
	shadertoy-shared)

# C++17
set_property(TARGET example30-compute PROPERTY CXX_STANDARD 17)

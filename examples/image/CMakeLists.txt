message(STATUS "Building example image")

add_executable(example_image
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${SRC_ROOT}/test.cpp)

target_include_directories(example_image PRIVATE
	${ST_INC_DIR}
	${INCLUDE_ROOT}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS})

target_link_libraries(example_image
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${Boost_LIBRARIES}
	${GLFW3_LIBRARIES}
	shadertoy-shared)

# C++17
set_property(TARGET example_image PROPERTY CXX_STANDARD 17)

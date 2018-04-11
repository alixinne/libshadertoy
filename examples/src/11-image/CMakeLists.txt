message(STATUS "Building example 11-image")

add_executable(example11-image
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_include_directories(example11-image PRIVATE
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS})

target_link_libraries(example11-image
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${GLFW3_LIBRARIES}
	shadertoy-shared)

# C++14
set_property(TARGET example11-image PROPERTY CXX_STANDARD 14)

message(STATUS "Building example 00-build")

add_executable(example00-build
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_include_directories(example00-build PRIVATE
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS})

target_link_libraries(example00-build
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${GLFW3_LIBRARIES}
	shadertoy-shared)

# C++14
set_property(TARGET example00-build PROPERTY CXX_STANDARD 14)

message(STATUS "Building example 20-geometry")

# Download tiny_obj_loader
file(DOWNLOAD https://raw.githubusercontent.com/syoyo/tinyobjloader/v1.0.6/tiny_obj_loader.h
	${INCLUDE_ROOT}/20-geometry/tiny_obj_loader.h
	EXPECTED_HASH SHA256=740c4e262a6ac56940852671bbe3e5e162cba7325c59ef57f3d9efdba2c5244c)
file(DOWNLOAD https://raw.githubusercontent.com/syoyo/tinyobjloader/v1.0.6/tiny_obj_loader.cc
	${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.cc
	EXPECTED_HASH SHA256=b27322737fbd97b615412efd404d20d4a9b0a6556cada15ca4b77c7447b38bd2)

# Download the stanford bunny
file(DOWNLOAD https://graphics.stanford.edu/~mdfisher/Data/Meshes/bunny.obj
	${SHADERS_ROOT}/bunny.obj
	EXPECTED_HASH SHA256=e4bfe098950c61c42190fefe8f23ad7b469da8d5d488c8f8e28a0e0b00c4c88c)

add_executable(example20-geometry
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/uniforms.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.cc
	${SRC_ROOT}/test.cpp)

target_include_directories(example20-geometry PRIVATE
	${INCLUDE_ROOT}
	${INCLUDE_ROOT}/20-geometry
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS})

target_link_libraries(example20-geometry
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${GLFW3_LIBRARIES}
	shadertoy-shared)

# C++14
set_property(TARGET example20-geometry PROPERTY CXX_STANDARD 14)

message(STATUS "Building example geometry")

# Download tiny_obj_loader
file(DOWNLOAD https://raw.githubusercontent.com/syoyo/tinyobjloader/v1.0.6/tiny_obj_loader.h
	${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.h
	EXPECTED_HASH SHA256=740c4e262a6ac56940852671bbe3e5e162cba7325c59ef57f3d9efdba2c5244c)
file(DOWNLOAD https://raw.githubusercontent.com/syoyo/tinyobjloader/v1.0.6/tiny_obj_loader.cc
	${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.cc
	EXPECTED_HASH SHA256=b27322737fbd97b615412efd404d20d4a9b0a6556cada15ca4b77c7447b38bd2)

# Download the stanford bunny (with normals)
file(DOWNLOAD https://casual-effects.com/g3d/data10/research/model/bunny/bunny.zip
	${CMAKE_CURRENT_BINARY_DIR}/bunny.zip
	EXPECTED_HASH SHA256=7082df1052c2a13263e5e83aee2b12af2345e2849592a69bfeebd0dc8cb46ef6)

# Extract model
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_BINARY_DIR}/bunny.zip
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/bunny.obj
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(example_geometry
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.cc
	${SRC_ROOT}/test.cpp)

target_include_directories(example_geometry PRIVATE
	${ST_INC_DIR}
	${INCLUDE_ROOT}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS})

target_link_libraries(example_geometry
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${Boost_LIBRARIES}
	${GLFW3_LIBRARIES}
	shadertoy-shared
	stbackend-gl4-shared)

# C++17
set_property(TARGET example_geometry PROPERTY CXX_STANDARD 17)

set_target_properties(example_geometry PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "Building example web-gradient")

add_executable(example_web_gradient ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(example_web_gradient
	shadertoy
	stbackend-webgl)

target_link_options(example_web_gradient PRIVATE
	"SHELL:-s USE_WEBGL2"
	"SHELL:-s EXIT_RUNTIME=1"
	"SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
	"SHELL:-s USE_GLFW=3"
	"SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/../gradient.glsl@gradient.glsl")

# C++17
set_property(TARGET example_web_gradient PROPERTY CXX_STANDARD 17)

set_target_properties(example_web_gradient PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

# Host page
add_custom_command(TARGET example_web_gradient POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_BINARY_DIR}/dist/gradient.html
	MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/index.html)

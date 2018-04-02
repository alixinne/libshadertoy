message(STATUS "Building example 50-shadertoy")

add_executable(example50-shadertoy
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_include_directories(example50-shadertoy PRIVATE
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLFW3_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
	${JSONCPP_INCLUDE_DIRS})

target_link_libraries(example50-shadertoy
	${OPENGL_LIBRARY}
	${EPOXY_LIBRARIES}
	${GLFW3_LIBRARIES}
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	${JSONCPP_LIBRARIES}
	shadertoy-shared)

target_compile_definitions(example50-shadertoy PUBLIC
	BOOST_LOG_DYN_LINK=1)

# C++14
set_property(TARGET example50-shadertoy PROPERTY CXX_STANDARD 14)

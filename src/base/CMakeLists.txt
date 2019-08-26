# spdlog requires a thread init library
find_package(Threads REQUIRED)

# Create config header
if (EMSCRIPTEN)
	set(SHADERTOY_HAS_GL_DESKTOP 0)
else()
	set(SHADERTOY_HAS_GL_DESKTOP 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/shadertoy/config.hpp.in
	${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/config.hpp @ONLY)

shadertoy_subtarget(stbase
	INTERFACE_INCLUDE_DIRS $<BUILD_INTERFACE:${SHADERTOY_BUILT_INCLUDE_DIR}>
	EXTRA_SOURCES ${SPDLOG_MAIN_INCLUDE}
	LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

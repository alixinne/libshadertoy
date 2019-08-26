cmake_minimum_required(VERSION 3.10)

project(libshadertoy-examples)

# Directories
set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if (EMSCRIPTEN)
	include(WebExamples.cmake)
else()
	include(DesktopExamples.cmake)
endif()

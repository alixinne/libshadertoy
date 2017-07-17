cmake_minimum_required(VERSION 2.8)

project(shadertoy)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

include(CMakeDependentOption)
include(cotire)
include(GNUInstallDirs)
include(GenerateExportHeader)

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

# Project directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Required packages
find_package(Boost 1.58 REQUIRED COMPONENTS filesystem log serialization date_time)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SOIL REQUIRED)

set(oglplus_CONFIGURE_EXTRA_ARGS --use-glew)
include(liboglplus)

# Copy shaders to source
file(GLOB RESOURCES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Resources.c
		   ${CMAKE_CURRENT_BINARY_DIR}/Resources.h
	DEPENDS ${RESOURCES_SOURCES}
	COMMAND ${CMAKE_COMMAND}
		-DRESOURCES_INPUT=${CMAKE_CURRENT_SOURCE_DIR}/shaders
		-DRESOURCES_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/Resources.c
		-DRESOURCES_H_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/Resources.h
		-DRESOURCES_H_ID=RESOURCES_H
		-P ${CMAKE_MODULE_PATH}/resources.cmake)

set(shadertoy_INCLUDE_DIRS
	${CMAKE_CURRENT_BINARY_DIR} # for Resources.h
	${INCLUDE_DIR})
set(shadertoy_LIBRARIES
	shadertoy)

# Main libshadertoy
add_library(shadertoy SHARED
	src/BufferConfig.cpp
	src/ContextConfig.cpp
	src/Misc.cpp
	src/RenderContext.cpp
	src/TextureEngine.cpp
	src/ToyBuffer.cpp
	src/UniformState.cpp
	${CMAKE_CURRENT_BINARY_DIR}/Resources.c)
generate_export_header(shadertoy
	BASE_NAME shadertoy
	EXPORT_MACRO_NAME shadertoy_EXPORT
	EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/shadertoy_export.h)

# Include directories
target_include_directories(shadertoy PRIVATE
	${Boost_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIRS}
	${oglplus_INCLUDE_DIRS}
	${SOIL_INCLUDE_DIRS}
	${shadertoy_INCLUDE_DIRS})

# Link dependencies
target_link_libraries(shadertoy
	${Boost_LIBRARIES}
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${SOIL_LIBRARIES})

# Fix Boost
target_compile_definitions(shadertoy PUBLIC
	BOOST_LOG_DYN_LINK=1)

# C++14
set_property(TARGET shadertoy PROPERTY CXX_STANDARD 14)

# Include directories for install
set_property(TARGET shadertoy APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
			 $<BUILD_INTERFACE:${INCLUDE_DIR}>
			 $<INSTALL_INTERFACE:include>
			 $<INSTALL_INTERFACE:${Boost_INCLUDE_DIR}>)

# Precompiled header optimization
set_target_properties(shadertoy PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT ${INCLUDE_DIR}/stdafx.hpp)
cotire(shadertoy)

# Change library properties
set_target_properties(shadertoy PROPERTIES
	OUTPUT_NAME shadertoy${VERSION_MAJOR}
	VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# Install target
install(TARGETS shadertoy
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		EXPORT shadertoy)

install(EXPORT shadertoy
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_DATADIR}/shadertoy)
install(FILES shadertoy-config.cmake
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_DATADIR}/shadertoy)
install(FILES modules/liboglplus.cmake
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_DATADIR}/shadertoy
		COMPONENT shadertoy-dev
		RENAME shadertoy-liboglplus.cmake)
install(DIRECTORY examples
		COMPONENT shadertoy-examples
		DESTINATION ${CMAKE_INSTALL_DATADIR}/shadertoy)

file(GLOB SHADERTOY_HEADERS ${INCLUDE_DIR}/shadertoy/*.hpp)
set(SHADERTOY_HEADERS ${SHADERTOY_HEADERS}
					  ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/shadertoy_export.h)
install(FILES ${SHADERTOY_HEADERS}
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy)

# Enable warnings
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

# Documentation
find_package(Doxygen)
cmake_dependent_option(SHADERTOY_BUILD_DOCUMENTATION
	"Build the HTML docs" ON
	"DOXYGEN_FOUND;LIBSHADERTOY_BUILD_DOCUMENTATION" OFF)

if(SHADERTOY_BUILD_DOCUMENTATION)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()

	set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
	set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/modules/Doxyfile.in)
	set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	configure_file(${doxyfile_in} ${doxyfile} @ONLY)

	add_custom_target(shadertoy_doc ALL
		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)

	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
			COMPONENT shadertoy-docs
			DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()

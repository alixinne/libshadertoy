cmake_minimum_required(VERSION 2.8)

project(shadertoy)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

include(CMakeDependentOption)
include(GNUInstallDirs)
include(GenerateExportHeader)

# Load lib version from debian/substvars
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/debian/substvars DEBIAN_SUBSTVARS)
string(REGEX MATCH "version=[0-9]+\\.[0-9]+\\.[0-9]+" DEBIAN_SUBSTVARS_VERSION_FIELD ${DEBIAN_SUBSTVARS})
string(REGEX REPLACE "^version=(.*)$" "\\1" ST_VERSION ${DEBIAN_SUBSTVARS_VERSION_FIELD})

# Parse the version string into major, minor and patch
string(REPLACE "." ";" ST_VERSION_LIST ${ST_VERSION})
list(GET ST_VERSION_LIST 0 VERSION_MAJOR)
list(GET ST_VERSION_LIST 1 VERSION_MINOR)
list(GET ST_VERSION_LIST 0 VERSION_PATCH)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION)

# Print a message with the version number
message("-- shadertoy v${VERSION}")

# Project directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Export compile commands for autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Required packages
find_package(Boost 1.58 REQUIRED COMPONENTS filesystem log date_time)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SOIL REQUIRED)
find_package(JPEG REQUIRED)

find_package(PkgConfig REQUIRED)

# Find GLM using CMake
find_package(glm QUIET)
if (NOT GLM_FOUND)
	# Find GLM using pkg-config
	pkg_search_module(GLM glm)

	if (NOT GLM_FOUND)
		# Find GLM using find_path
		find_path(GLM_INCLUDE_DIRS glm/glm.hpp)

		if (NOT GLM_INCLUDE_DIRS)
			message(FATAL "Could not find GLM using any of CMake, pkg-config or find_path. Please ensure libglm-dev is installed.")
		endif()
	endif()
endif()

# Find libunwind for printing backtraces on GL errors
pkg_search_module(UNWIND libunwind)

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
	src/RenderContext.cpp
	src/ShaderCompiler.cpp
	src/ShadertoyError.cpp
	src/TextureEngine.cpp
	src/ToyBuffer.cpp
	src/UniformState.cpp
	src/OpenGL/Buffer.cpp
	src/OpenGL/Caller.cpp
	src/OpenGL/Framebuffer.cpp
	src/OpenGL/Program.cpp
	src/OpenGL/Renderbuffer.cpp
	src/OpenGL/Shader.cpp
	src/OpenGL/Texture.cpp
	${CMAKE_CURRENT_BINARY_DIR}/Resources.c)

generate_export_header(shadertoy
	BASE_NAME shadertoy
	EXPORT_MACRO_NAME shadertoy_EXPORT
	EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/shadertoy_export.h)

# Include directories
target_include_directories(shadertoy PRIVATE
	${shadertoy_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${SOIL_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIRS})

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
    set(UNWIND_ARCH "x86_64")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^i.86$")
    set(UNWIND_ARCH "x86")
else()
	if (UNWIND_FOUND)
		message("-- Unsupported architecture for libunwind")
		set(UNWIND_ARCH OFF)
	endif()
endif()

# Add UNWIND if found
if (UNWIND_FOUND AND UNWIND_ARCH)
	message("-- Using unwind for tracing OpenGL calls")

	target_link_libraries(shadertoy ${UNWIND_LIBRARIES} unwind-${UNWIND_ARCH})
	target_include_directories(shadertoy PRIVATE ${UNWIND_INCLUDE_DIRS})
	target_compile_definitions(shadertoy PRIVATE HAS_UNWIND=1 UNW_LOCAL_ONLY=1)
else()
	message("-- Not using unwind for tracing")
	target_compile_definitions(shadertoy PRIVATE HAS_UNWIND=0)
endif()

# Link dependencies
target_link_libraries(shadertoy
	${Boost_LIBRARIES}
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${JPEG_LIBRARIES}
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

# Set library file version
set_target_properties(shadertoy PROPERTIES
	OUTPUT_NAME shadertoy${VERSION_MAJOR}
	VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# Install target
install(TARGETS shadertoy
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		EXPORT shadertoy)
install(EXPORT shadertoy
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shadertoy)

install(EXPORT shadertoy
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_DATADIR}/shadertoy)

# Configure the -config.cmake file
configure_file(shadertoy-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/shadertoy-config.cmake @ONLY)
# Configure -version.cmake file
configure_file(shadertoy-config-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/shadertoy-config-version.cmake @ONLY)

# Install cmake config files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/shadertoy-config.cmake
			  ${CMAKE_CURRENT_BINARY_DIR}/shadertoy-config-version.cmake
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shadertoy)

# Install the examples
install(DIRECTORY examples
		COMPONENT shadertoy-examples
		DESTINATION ${CMAKE_INSTALL_DATADIR}/shadertoy)

# Install headers
file(GLOB SHADERTOY_HEADERS ${INCLUDE_DIR}/shadertoy/*.hpp)
set(SHADERTOY_HEADERS ${SHADERTOY_HEADERS}
					  ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/shadertoy_export.h)
install(FILES ${SHADERTOY_HEADERS}
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy)

file(GLOB SHADERTOY_GL_HEADERS ${INCLUDE_DIR}/shadertoy/OpenGL/*.hpp)
install(FILES ${SHADERTOY_GL_HEADERS}
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy/OpenGL)

# Enable warnings
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-missing-braces -pedantic")
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

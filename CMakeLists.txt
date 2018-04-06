cmake_minimum_required(VERSION 3.1)

project(shadertoy)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

include(CMakeDependentOption)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

# Load lib version from debian/substvars
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog DEBIAN_CHANGELOG
	REGEX "^libshadertoy \\(.*\\) ")
list(GET DEBIAN_CHANGELOG 0 DEBIAN_CHANGELOG_HEAD)
string(REGEX REPLACE "^.*\\(([0-9]+\\.[0-9]+\\.[0-9]+)(-.*)?\\).*$" "\\1" ST_VERSION ${DEBIAN_CHANGELOG_HEAD})

# Parse the version string into major, minor and patch
string(REPLACE "." ";" ST_VERSION_LIST ${ST_VERSION})
list(GET ST_VERSION_LIST 0 VERSION_MAJOR)
list(GET ST_VERSION_LIST 1 VERSION_MINOR)
list(GET ST_VERSION_LIST 2 VERSION_PATCH)
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
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem log date_time)
find_package(OpenGL REQUIRED)
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
if (UNWIND_FOUND)
	link_directories(${UNWIND_LIBDIR})
endif()

# libepoxy extension loader
pkg_search_module(EPOXY REQUIRED epoxy)
link_directories(${EPOXY_LIBDIR})

# Copy shaders to source
file(GLOB RESOURCES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.c
		   ${CMAKE_CURRENT_BINARY_DIR}/resources.h
	DEPENDS ${RESOURCES_SOURCES}
	COMMAND ${CMAKE_COMMAND}
		-DRESOURCES_INPUT=${CMAKE_CURRENT_SOURCE_DIR}/shaders
		-DRESOURCES_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/resources.c
		-DRESOURCES_H_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/resources.h
		-DRESOURCES_H_ID=RESOURCES_H
		-P ${CMAKE_MODULE_PATH}/resources.cmake)

set(shadertoy_INCLUDE_DIRS
	${CMAKE_CURRENT_BINARY_DIR} # for Resources.h
	$<BUILD_INTERFACE:${INCLUDE_DIR}>
	$<INSTALL_INTERFACE:include>)

add_library(shadertoy-objects OBJECT
	${INCLUDE_DIR}/shadertoy.hpp
	${INCLUDE_DIR}/shadertoy/pre.hpp
	${INCLUDE_DIR}/shadertoy/buffer_config.hpp
	${INCLUDE_DIR}/shadertoy/context_config.hpp
	${INCLUDE_DIR}/shadertoy/render_context.hpp
	${INCLUDE_DIR}/shadertoy/shader_compiler.hpp
	${INCLUDE_DIR}/shadertoy/shadertoy_error.hpp
	${INCLUDE_DIR}/shadertoy/texture_engine.hpp
	${INCLUDE_DIR}/shadertoy/toy_buffer.hpp
	${INCLUDE_DIR}/shadertoy/uniform_state.hpp
	${INCLUDE_DIR}/shadertoy/uniform_state_decl.hpp
	${INCLUDE_DIR}/shadertoy/uniform_state_impl.hpp
	${INCLUDE_DIR}/shadertoy/gl.hpp
	${INCLUDE_DIR}/shadertoy/gl/query.hpp
	${INCLUDE_DIR}/shadertoy/gl/renderbuffer.hpp
	${INCLUDE_DIR}/shadertoy/gl/caller.hpp
	${INCLUDE_DIR}/shadertoy/gl/buffer.hpp
	${INCLUDE_DIR}/shadertoy/gl/program.hpp
	${INCLUDE_DIR}/shadertoy/gl/framebuffer.hpp
	${INCLUDE_DIR}/shadertoy/gl/resource.hpp
	${INCLUDE_DIR}/shadertoy/gl/texture.hpp
	${INCLUDE_DIR}/shadertoy/gl/shader.hpp
	${SRC_DIR}/buffer_config.cpp
	${SRC_DIR}/context_config.cpp
	${SRC_DIR}/render_context.cpp
	${SRC_DIR}/shader_compiler.cpp
	${SRC_DIR}/shadertoy_error.cpp
	${SRC_DIR}/texture_engine.cpp
	${SRC_DIR}/toy_buffer.cpp
	${SRC_DIR}/uniform_state.cpp
	${SRC_DIR}/gl/buffer.cpp
	${SRC_DIR}/gl/caller.cpp
	${SRC_DIR}/gl/framebuffer.cpp
	${SRC_DIR}/gl/program.cpp
	${SRC_DIR}/gl/query.cpp
	${SRC_DIR}/gl/renderbuffer.cpp
	${SRC_DIR}/gl/shader.cpp
	${SRC_DIR}/gl/texture.cpp
	${SRC_DIR}/utils/dump_program.cpp
	${CMAKE_CURRENT_BINARY_DIR}/resources.c)

# Include directories
target_include_directories(shadertoy-objects PRIVATE
	${shadertoy_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${SOIL_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIRS})

# Fix Boost
target_compile_definitions(shadertoy-objects PUBLIC
	BOOST_LOG_DYN_LINK=1)

# C++14
set(CXX_STANDARD_REQUIRED ON)
set_property(TARGET shadertoy-objects PROPERTY CXX_STANDARD 14)

# -fPIC
set_property(TARGET shadertoy-objects PROPERTY POSITION_INDEPENDENT_CODE ON)

# Add UNWIND if found
execute_process(COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -dumpmachine
				OUTPUT_VARIABLE COMPILER_ARCH
				OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "-" ";" COMPILER_ARCH_LIST ${COMPILER_ARCH})
list(GET COMPILER_ARCH_LIST 0 COMPILER_ARCH_NAME)
string(REGEX REPLACE "i.86" "x86" UNWIND_ARCH ${COMPILER_ARCH_NAME})

if (UNWIND_FOUND AND UNWIND_ARCH)
	message("-- Using unwind for tracing OpenGL calls")

	target_include_directories(shadertoy-objects PRIVATE ${UNWIND_INCLUDE_DIRS})
	target_compile_definitions(shadertoy-objects PRIVATE HAS_UNWIND=1 UNW_LOCAL_ONLY=1)
else()
	message("-- Not using unwind for tracing")
	target_compile_definitions(shadertoy-objects PRIVATE HAS_UNWIND=0)
endif()

# Build static and shared libraries
set(LIBRARY_TYPES "SHARED;STATIC")
foreach(LIBRARY_TYPE ${LIBRARY_TYPES})
	set(TARGET_NAME "shadertoy-${LIBRARY_TYPE}")
	string(TOLOWER ${TARGET_NAME} TARGET_NAME)
	message(STATUS "Creating target ${TARGET_NAME}")

	# Main libshadertoy
	add_library(${TARGET_NAME} ${LIBRARY_TYPE}
		$<TARGET_OBJECTS:shadertoy-objects>)

	# Add UNWIND if found
	if (UNWIND_FOUND AND UNWIND_ARCH)
		target_link_libraries(${TARGET_NAME} PRIVATE ${UNWIND_LIBRARIES} unwind-${UNWIND_ARCH})
	endif()

	# Link dependencies
	target_link_libraries(${TARGET_NAME} PUBLIC
		${Boost_LIBRARIES}
		${OPENGL_LIBRARY}
		${EPOXY_LIBRARIES}
		${JPEG_LIBRARIES}
		${SOIL_LIBRARIES})

	# Include directories for install
	target_include_directories(${TARGET_NAME} INTERFACE $<INSTALL_INTERFACE:include>)

	# Set library file version
	set_target_properties(${TARGET_NAME} PROPERTIES
		SOVERSION ${VERSION_MAJOR}
		VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

	if(NOT WIN32)
		set_target_properties(${TARGET_NAME} PROPERTIES
			OUTPUT_NAME shadertoy)
	endif()

	# Install target
	install(TARGETS ${TARGET_NAME}
			DESTINATION ${CMAKE_INSTALL_LIBDIR}
			EXPORT shadertoy)
	install(EXPORT shadertoy
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shadertoy)

	install(EXPORT shadertoy
			COMPONENT shadertoy-dev
			DESTINATION ${CMAKE_INSTALL_DATADIR}/shadertoy)
endforeach()

# Export header for shared
generate_export_header(shadertoy-shared
	BASE_NAME shadertoy
	EXPORT_MACRO_NAME shadertoy_EXPORT
	EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/shadertoy_export.h)

# Configure the -config.cmake file
configure_file(shadertoy-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/shadertoy-config.cmake @ONLY)
# Configure -version.cmake file
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/shadertoy-config-version.cmake
	VERSION ${VERSION}
	COMPATIBILITY SameMajorVersion)

# Install cmake config files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/shadertoy-config.cmake
			  ${CMAKE_CURRENT_BINARY_DIR}/shadertoy-config-version.cmake
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shadertoy)

# Install the examples
install(DIRECTORY examples
		COMPONENT shadertoy-examples
		DESTINATION ${CMAKE_INSTALL_DATADIR}/shadertoy
		PATTERN ".idea" EXCLUDE
		PATTERN "build" EXCLUDE
		PATTERN "*.dump" EXCLUDE)

# Install headers
install(FILES ${INCLUDE_DIR}/shadertoy.hpp
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

file(GLOB SHADERTOY_HEADERS ${INCLUDE_DIR}/shadertoy/*.hpp)
list(APPEND SHADERTOY_HEADERS
		${CMAKE_CURRENT_BINARY_DIR}/shadertoy/shadertoy_export.h)
install(FILES ${SHADERTOY_HEADERS}
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy)

file(GLOB SHADERTOY_GL_HEADERS ${INCLUDE_DIR}/shadertoy/gl/*.hpp)
install(FILES ${SHADERTOY_GL_HEADERS}
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy/gl)

file(GLOB SHADERTOY_UTILS_HEADERS ${INCLUDE_DIR}/shadertoy/utils/*.hpp)
install(FILES ${SHADERTOY_UTILS_HEADERS}
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy/utils)

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

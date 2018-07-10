cmake_minimum_required(VERSION 3.1)

project(shadertoy LANGUAGES CXX)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

include(CMakeDependentOption)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

# Load lib version from debian/substvars
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog DEBIAN_CHANGELOG
	REGEX "^libshadertoy \\(.*\\) ")
list(GET DEBIAN_CHANGELOG 0 DEBIAN_CHANGELOG_HEAD)
string(REGEX REPLACE "^.*\\(([0-9]+\\.[0-9]+\\.[0-9]+)([-~].*)?\\).*$" "\\1" ST_VERSION ${DEBIAN_CHANGELOG_HEAD})
string(REGEX REPLACE "^.*\\(([0-9]+\\.[0-9]+\\.[0-9]+)([-~].*)?\\).*$" "\\1\\2"
	ST_VERSION_FULL ${DEBIAN_CHANGELOG_HEAD})

# Parse the version string into major, minor and patch
string(REPLACE "." ";" ST_VERSION_LIST ${ST_VERSION})
list(GET ST_VERSION_LIST 0 VERSION_MAJOR)
list(GET ST_VERSION_LIST 1 VERSION_MINOR)
list(GET ST_VERSION_LIST 2 VERSION_PATCH)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION)

# Check if we are running in standalone build or in-source
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	option(SHADERTOY_STANDALONE "Set to ON when libshadertoy is being built from source instead of in a project" ON)
else()
	option(SHADERTOY_STANDALONE "Set to ON when libshadertoy is being built from source instead of in a project" OFF)
endif()

if (SHADERTOY_STANDALONE)
	set(SHADERTOY_VERSION_STANDALONE "Standalone build")
	set(SHADERTOY_DEFAULT_TARGET_ARG "")
else()
	set(SHADERTOY_VERSION_STANDALONE "In-source build")
	set(SHADERTOY_DEFAULT_TARGET_ARG EXCLUDE_FROM_ALL)
endif()

# Print a message with the version number
message(STATUS "shadertoy v${ST_VERSION_FULL} (${SHADERTOY_VERSION_STANDALONE})")

# Project directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Export compile commands for autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Required packages
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem date_time)
find_package(OpenGL REQUIRED)

find_package(PkgConfig REQUIRED)

# Find GLM using CMake
find_package(glm REQUIRED)

# Find libunwind for printing backtraces on GL errors
find_package(Unwind)

# libepoxy extension loader
find_package(Epoxy REQUIRED)

# Use bundled spdlog
set(SPDLOG_BUILD_EXAMPLES OFF)
set(SPDLOG_BUILD_TESTING OFF)
set(SPDLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog)
add_subdirectory(${SPDLOG_DIR} EXCLUDE_FROM_ALL)

# Copy headers to include directory for build
set(SPDLOG_MAIN_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/spdlog/spdlog.h)
add_custom_command(OUTPUT ${SPDLOG_MAIN_INCLUDE}
				   MAIN_DEPENDENCY ${SPDLOG_DIR}/include/spdlog/spdlog.h
				   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/spdlog
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${SPDLOG_DIR}/include/spdlog ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/spdlog)

# uri.hh
set(URI_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uri-library)
set(URI_LIBRARY_MAIN_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/uri.hpp)
add_custom_command(OUTPUT ${URI_LIBRARY_MAIN_INCLUDE}
				   MAIN_DEPENDENCY ${URI_LIBRARY_DIR}/uri.hh
				   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shadertoy
				   COMMAND ${CMAKE_COMMAND} -E copy ${URI_LIBRARY_DIR}/uri.hh ${URI_LIBRARY_MAIN_INCLUDE})

# libsoil
find_package(SOIL)

# libjpeg
find_package(JPEG)

# libopenexr
find_package(OpenEXR)

# Copy shaders to source
file(GLOB RESOURCES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.cpp
		   ${CMAKE_CURRENT_BINARY_DIR}/resources.hpp
	DEPENDS ${RESOURCES_SOURCES}
	COMMAND ${CMAKE_COMMAND}
		-DRESOURCES_INPUT=${CMAKE_CURRENT_SOURCE_DIR}/shaders
		-DRESOURCES_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/resources.cpp
		-DRESOURCES_H_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/resources.hpp
		-DRESOURCES_H_ID=_SHADERTOY_RESOURCES_HPP_
		-P ${CMAKE_MODULE_PATH}/resources.cmake)

set(shadertoy_INCLUDE_DIRS
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for Resources.h
	$<BUILD_INTERFACE:${INCLUDE_DIR}>
	$<INSTALL_INTERFACE:include>)

file(GLOB_RECURSE SHADERTOY_LIBRARY_HEADERS ${INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE SHADERTOY_LIBRARY_SOURCES ${SRC_DIR}/*.cpp)

add_library(shadertoy-objects OBJECT EXCLUDE_FROM_ALL
	${SHADERTOY_LIBRARY_HEADERS}
	${SHADERTOY_LIBRARY_SOURCES}

	${CMAKE_CURRENT_BINARY_DIR}/resources.cpp
	${POLYMORPHIC_VALUE_INCLUDE}
	${SPDLOG_MAIN_INCLUDE}
	${URI_LIBRARY_MAIN_INCLUDE})

# Enable warnings
if(MSVC)
	target_compile_options(shadertoy-objects PRIVATE /W2 /wd4275 /wd4251)
else()
	target_compile_options(shadertoy-objects PRIVATE -Wall -Wno-missing-braces -Wno-attributes -pedantic)
endif()

# Include directories
target_include_directories(shadertoy-objects PRIVATE
	${shadertoy_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS})

if (SOIL_FOUND)
	message(STATUS "Building with SOIL support")
	target_include_directories(shadertoy-objects PRIVATE
		${SOIL_INCLUDE_DIRS})
	target_compile_definitions(shadertoy-objects PRIVATE
		LIBSHADERTOY_SOIL=1)
else()
	message(STATUS "Building without SOIL support")
	target_compile_definitions(shadertoy-objects PRIVATE
		LIBSHADERTOY_SOIL=0)
endif()

if (JPEG_FOUND)
	message(STATUS "Building with JPEG support")
	target_include_directories(shadertoy-objects PRIVATE
		${JPEG_INCLUDE_DIRS})
	target_compile_definitions(shadertoy-objects PRIVATE
		LIBSHADERTOY_JPEG=1)
else()
	message(STATUS "Building without SOIL support")
	target_compile_definitions(shadertoy-objects PRIVATE
		LIBSHADERTOY_JPEG=0)
endif()

if (OpenEXR_FOUND)
	message(STATUS "Building with OpenEXR support")
	target_include_directories(shadertoy-objects PRIVATE
		${OpenEXR_INCLUDE_DIRS})
	target_compile_definitions(shadertoy-objects PRIVATE
		LIBSHADERTOY_OPENEXR=1)
else()
	message(STATUS "Building without OpenEXR support")
	target_compile_definitions(shadertoy-objects PRIVATE
		LIBSHADERTOY_OPENEXR=0)
endif()

# C++14
set(CXX_STANDARD_REQUIRED ON)
set_property(TARGET shadertoy-objects PROPERTY CXX_STANDARD 14)

# -fPIC
set_property(TARGET shadertoy-objects PROPERTY POSITION_INDEPENDENT_CODE ON)

# Enable Unwind support
if (UNWIND_FOUND)
	message(STATUS "Using unwind for tracing OpenGL calls")

	target_include_directories(shadertoy-objects PRIVATE ${UNWIND_INCLUDE_DIRS})
	target_compile_definitions(shadertoy-objects PRIVATE HAS_UNWIND=1 UNW_LOCAL_ONLY=1)
else()
	message(STATUS "Not using unwind for tracing")
	target_compile_definitions(shadertoy-objects PRIVATE HAS_UNWIND=0)
endif()

# Build static and shared libraries
set(LIBRARY_TYPES "SHARED;STATIC")
foreach(LIBRARY_TYPE ${LIBRARY_TYPES})
	set(TARGET_NAME "shadertoy-${LIBRARY_TYPE}")
	string(TOLOWER ${TARGET_NAME} TARGET_NAME)
	message(STATUS "Creating target ${TARGET_NAME}")

	# Main libshadertoy
	add_library(${TARGET_NAME} ${LIBRARY_TYPE} ${SHADERTOY_DEFAULT_TARGET_ARG}
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

	if (SOIL_FOUND)
		target_link_libraries(${TARGET_NAME} PUBLIC ${SOIL_LIBRARIES})
	endif()

	if (JPEG_FOUND)
		target_link_libraries(${TARGET_NAME} PUBLIC ${JPEG_LIBRARIES})
	endif()

	if (OpenEXR_FOUND)
		target_link_libraries(${TARGET_NAME} PUBLIC ${OpenEXR_LIBRARIES})
	endif()

	# Include directories for install
	target_include_directories(${TARGET_NAME} INTERFACE
		${shadertoy_INCLUDE_DIRS}
		${Boost_INCLUDE_DIR}
		${GLM_INCLUDE_DIRS})

	# Set library file version
	set_target_properties(${TARGET_NAME} PROPERTIES
		SOVERSION ${VERSION_MAJOR}
		VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

	if (SHADERTOY_STANDALONE)
		# Install target
		install(TARGETS ${TARGET_NAME}
				DESTINATION ${CMAKE_INSTALL_LIBDIR}
				EXPORT shadertoy)
	endif()
endforeach()

set_target_properties(shadertoy-shared PROPERTIES OUTPUT_NAME shadertoy)

if (SHADERTOY_STANDALONE)
	# Export targets
	export(EXPORT shadertoy FILE ${CMAKE_CURRENT_BINARY_DIR}/shadertoy.cmake)

	# Export build package
	export(PACKAGE shadertoy)

	# Install exports
	install(EXPORT shadertoy
			COMPONENT shadertoy-dev
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shadertoy)

	# Export header for shared
	generate_export_header(shadertoy-shared
		BASE_NAME shadertoy
		EXPORT_MACRO_NAME shadertoy_EXPORT
		EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/shadertoy_export.h)
	target_compile_definitions(shadertoy-objects PUBLIC shadertoy_shared_EXPORTS)

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

	file(GLOB SHADERTOY_HEADERS_DIRS RELATIVE ${INCLUDE_DIR}/shadertoy ${INCLUDE_DIR}/shadertoy/*)
	foreach(SHADERTOY_HEADERS_ITEM ${SHADERTOY_HEADERS_DIRS})
		if(IS_DIRECTORY ${INCLUDE_DIR}/shadertoy/${SHADERTOY_HEADERS_ITEM})
			file(GLOB SHADERTOY_HEADER_FILES ${INCLUDE_DIR}/shadertoy/${SHADERTOY_HEADERS_ITEM}/*.hpp)
			install(FILES ${SHADERTOY_HEADER_FILES}
				    COMPONENT shadertoy-dev
					DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy/${SHADERTOY_HEADERS_ITEM})
		endif()
	endforeach()

	install(DIRECTORY ${SPDLOG_DIR}/include/spdlog
			COMPONENT shadertoy-dev
			DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy)
endif()

# Documentation
find_package(Doxygen)
cmake_dependent_option(SHADERTOY_BUILD_DOCUMENTATION
	"Build the HTML docs" ON
	"DOXYGEN_FOUND" OFF)

if(SHADERTOY_BUILD_DOCUMENTATION)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()

	#set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
	set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
	set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
	set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	configure_file(${doxyfile_in} ${doxyfile} @ONLY)

	if (LIBSHADERTOY_BUILD_DOCUMENTATION)
		set(DOC_ARG_MODE ALL)
	else()
		set(DOC_ARG_MODE "")
	endif()

	add_custom_target(shadertoy_doc ${DOC_ARG_MODE}
		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)

	if (LIBSHADERTOY_BUILD_DOCUMENTATION)
		install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
				COMPONENT shadertoy-docs
				DESTINATION ${CMAKE_INSTALL_DOCDIR})
	endif()
endif()

if(SHADERTOY_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

# Package building
set(CPACK_GENERATOR "7Z")
execute_process(COMMAND git rev-parse --short HEAD
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_REVISION
	OUTPUT_STRIP_TRAILING_WHITESPACE
	ERROR_QUIET)
set(CPACK_PACKAGE_FILE_NAME "libshadertoy-${ST_VERSION_FULL}-${GIT_REVISION}")
include(CPack)

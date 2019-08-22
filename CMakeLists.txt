cmake_minimum_required(VERSION 3.10)

project(shadertoy LANGUAGES CXX)
set(SHADERTOY_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/modules")
set(CMAKE_MODULE_PATH ${SHADERTOY_CMAKE_DIR})

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

set(_library_types SHARED STATIC)
set(SHADERTOY_BUILT_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(SHADERTOY_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(SHADERTOY_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)

# Print a message with the version number
message(STATUS "shadertoy v${ST_VERSION_FULL} (${SHADERTOY_VERSION_STANDALONE})")

# Export compile commands for autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use bundled spdlog
set(SPDLOG_BUILD_EXAMPLES OFF)
set(SPDLOG_BUILD_TESTING OFF)
set(SPDLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog)
add_subdirectory(${SPDLOG_DIR} EXCLUDE_FROM_ALL)

# Copy headers to include directory for build
set(SPDLOG_MAIN_INCLUDE ${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/spdlog/spdlog.h)
add_custom_command(OUTPUT ${SPDLOG_MAIN_INCLUDE}
				   MAIN_DEPENDENCY ${SPDLOG_DIR}/include/spdlog/spdlog.h
				   COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/spdlog
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${SPDLOG_DIR}/include/spdlog ${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/spdlog)

# Copy headers initially
if(NOT EXISTS ${SPDLOG_MAIN_INCLUDE})
	execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/spdlog)
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${SPDLOG_DIR}/include/spdlog ${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/spdlog)
endif()

# Sources
add_subdirectory(src)

# Export header for shared targets
generate_export_header(shadertoy-shared
	BASE_NAME shadertoy
	EXPORT_MACRO_NAME shadertoy_EXPORT
	EXPORT_FILE_NAME ${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/shadertoy_export.h)

# Documentation
set(SHADERTOY_DOC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(docs)

# CMake config files
if (SHADERTOY_STANDALONE)
	# Export targets
	export(EXPORT shadertoy FILE ${CMAKE_CURRENT_BINARY_DIR}/shadertoy.cmake)

	# Export build package
	export(PACKAGE shadertoy)

	# Install exports
	install(EXPORT shadertoy
			COMPONENT shadertoy-dev
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shadertoy)

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

	# Install extra headers
	install(DIRECTORY ${SPDLOG_DIR}/include/spdlog
			COMPONENT shadertoy-dev
			DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy)

	install(FILES ${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/shadertoy_export.h
			COMPONENT shadertoy-dev
			DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy)
endif()

# Build examples
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

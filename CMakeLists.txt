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
message(STATUS "shadertoy v${VERSION}")

# Project directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Export compile commands for autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Required packages
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem date_time)
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

# Use bundled spdlog
set(SPDLOG_BUILD_EXAMPLES OFF)
set(SPDLOG_BUILD_TESTING OFF)
set(SPDLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog)
add_subdirectory(${SPDLOG_DIR} EXCLUDE_FROM_ALL)

# Copy headers to include directory for build
set(SPDLOG_MAIN_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/spdlog/spdlog.h)
add_custom_command(OUTPUT ${SPDLOG_MAIN_INCLUDE}
				   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/spdlog
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${SPDLOG_DIR}/include/spdlog ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/spdlog)

set(POLYMORPHIC_VALUE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/polymorphic_value)
add_subdirectory(${POLYMORPHIC_VALUE_DIR} EXCLUDE_FROM_ALL)

set(POLYMORPHIC_VALUE_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/shadertoy/polymorphic_value.h)
add_custom_command(OUTPUT ${POLYMORPHIC_VALUE_INCLUDE}
				   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shadertoy
				   COMMAND ${CMAKE_COMMAND} -E copy ${POLYMORPHIC_VALUE_DIR}/polymorphic_value.h ${CMAKE_CURRENT_BINARY_DIR}/shadertoy)

# libopenexr
pkg_search_module(OpenEXR OpenEXR)
if (OpenEXR_FOUND)
	link_directories(${OpenEXR_LIBDIR})
endif()

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

file(GLOB_RECURSE SHADERTOY_LIBRARY_HEADERS ${INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE SHADERTOY_LIBRARY_SOURCES ${SRC_DIR}/*.cpp)

add_library(shadertoy-objects OBJECT
	${SHADERTOY_LIBRARY_HEADERS}
	${SHADERTOY_LIBRARY_SOURCES}

	${CMAKE_CURRENT_BINARY_DIR}/resources.c
	${POLYMORPHIC_VALUE_INCLUDE}
	${SPDLOG_MAIN_INCLUDE})

# Include directories
target_include_directories(shadertoy-objects PRIVATE
	${shadertoy_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${SOIL_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIRS})

if (OpenEXR_FOUND)
	message("-- Building with OpenEXR support")
	target_include_directories(shadertoy-objects PRIVATE
		${OpenEXR_INCLUDE_DIRS})
	target_compile_definitions(shadertoy-objects PRIVATE
		LIBSHADERTOY_OPENEXR=1)
else()
	message("-- Building without OpenEXR support")
	target_compile_definitions(shadertoy-objects PRIVATE
		LIBSHADERTOY_OPENEXR=0)
endif()

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
	add_library(${TARGET_NAME} ${LIBRARY_TYPE}
		$<TARGET_OBJECTS:shadertoy-objects>)

	# Add UNWIND if found
	if (UNWIND_FOUND AND UNWIND_ARCH)
		target_link_libraries(${TARGET_NAME} PRIVATE ${UNWIND_LIBRARIES} unwind-${UNWIND_ARCH})
	endif()

	# Add OpenEXR define
	if (OpenEXR_FOUND)
		target_compile_definitions(${TARGET_NAME} INTERFACE LIBSHADERTOY_OPENEXR=1)
	else()
		target_compile_definitions(${TARGET_NAME} INTERFACE LIBSHADERTOY_OPENEXR=0)
	endif()

	# Link dependencies
	target_link_libraries(${TARGET_NAME} PUBLIC
		${Boost_LIBRARIES}
		${OPENGL_LIBRARY}
		${EPOXY_LIBRARIES}
		${JPEG_LIBRARIES}
		${SOIL_LIBRARIES})

	if (OpenEXR_FOUND)
		target_link_libraries(${TARGET_NAME} PUBLIC
			${OpenEXR_LIBRARIES})
	endif()

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

install(FILES ${POLYMORPHIC_VALUE_DIR}/polymorphic_value.h
		COMPONENT shadertoy-dev
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shadertoy)

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

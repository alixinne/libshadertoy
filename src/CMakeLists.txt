function(shadertoy_subtarget base_target_name)
	cmake_parse_arguments(_shadertoy_subtarget
		""
		""
		"INTERFACE_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;SUBTARGET_DEPENDENCIES;LINK_LIBRARIES;EXTRA_SOURCES;DEFINITIONS"
		${ARGN})

	set(_shadertoy_subtarget_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

	# Subtarget directories
	set(${base_target_name}_INCLUDE_DIR ${_shadertoy_subtarget_DIRECTORY}/include)
	set(${base_target_name}_SRC_DIR ${_shadertoy_subtarget_DIRECTORY}/src)

	# Subtarget sources
	file(GLOB_RECURSE ${base_target_name}_SOURCES
		${${base_target_name}_SRC_DIR}/*.c
		${${base_target_name}_SRC_DIR}/*.cc
		${${base_target_name}_SRC_DIR}/*.cpp
		${${base_target_name}_SRC_DIR}/*.cxx)

	file(GLOB_RECURSE ${base_target_name}_HEADERS
		${${base_target_name}_INCLUDE_DIR}/*.h
		${${base_target_name}_INCLUDE_DIR}/*.hpp
		${${base_target_name}_INCLUDE_DIR}/*.hxx
		${${base_target_name}_INCLUDE_DIR}/*.ipp)

	# Dependencies interface
	set(${base_target_name}_DEPS_INCLUDE_DIRS "")
	foreach(_subtarget_dep ${_shadertoy_subtarget_SUBTARGET_DEPENDENCIES})
		if (_library_types_count GREATER 1)
			set(_target_suffix "-shared")
		else()
			set(_target_suffix "")
		endif()
		get_target_property(_subtarget_include ${_subtarget_dep}${_target_suffix} INTERFACE_INCLUDE_DIRECTORIES)
		list(APPEND ${base_target_name}_DEPS_INCLUDE_DIRS ${_subtarget_include})
	endforeach()

	# Subtarget object library
	add_library(${base_target_name}-objects OBJECT EXCLUDE_FROM_ALL
		${${base_target_name}_SOURCES}
		${${base_target_name}_HEADERS}
		${_shadertoy_subtarget_EXTRA_SOURCES})

	# C++17
	target_compile_features(${base_target_name}-objects PUBLIC cxx_std_17)

	# -fPIC
	set_property(TARGET ${base_target_name}-objects PROPERTY POSITION_INDEPENDENT_CODE ON)

	# Compile definitions
	target_compile_definitions(${base_target_name}-objects PRIVATE ${_shadertoy_subtarget_DEFINITIONS})

	# Private include directory
	target_include_directories(${base_target_name}-objects PRIVATE
		${${base_target_name}_INCLUDE_DIR}
		${${base_target_name}_DEPS_INCLUDE_DIRS}
		${CMAKE_CURRENT_BINARY_DIR}
		${SHADERTOY_BUILT_INCLUDE_DIR}
		${_shadertoy_subtarget_INTERFACE_INCLUDE_DIRS}
		${_shadertoy_subtarget_PRIVATE_INCLUDE_DIRS})

	# Export header for shared targets
	string(REPLACE "-" "_" base_target_name_macro "${base_target_name}")
	generate_export_header(${base_target_name}-objects
		BASE_NAME ${base_target_name}
		EXPORT_MACRO_NAME ${base_target_name_macro}_EXPORT
		EXPORT_FILE_NAME ${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/${base_target_name}_export.h)

	# Export symbols
	target_compile_definitions(${base_target_name}-objects PRIVATE ${base_target_name_macro}_objects_EXPORTS)

	foreach(_library_type ${_library_types})
		if (_library_types_count GREATER 1)
			string(TOLOWER "-${_library_type}" _target_suffix)
		else()
			set(_target_suffix "")
		endif()

		set(_target_name "${base_target_name}${_target_suffix}")
		message(STATUS "Creating target ${_target_name}")

		# Create library target
		add_library(${_target_name} ${_library_type} ${SHADERTOY_DEFAULT_TARGET_ARG_${_library_type}}
			$<TARGET_OBJECTS:${base_target_name}-objects>)

		# Create list of libraries to link
		set(${_target_name}_LINK_LIBRARIES ${_shadertoy_subtarget_LINK_LIBRARIES})
		foreach(_subtarget_dep ${_shadertoy_subtarget_SUBTARGET_DEPENDENCIES})
			string(TOLOWER ${_subtarget_dep}${_target_suffix} _subtarget_lib)
			list(APPEND ${_target_name}_LINK_LIBRARIES ${_subtarget_lib})
		endforeach()

		if(${_target_name}_LINK_LIBRARIES)
			target_link_libraries(${_target_name} PUBLIC ${${_target_name}_LINK_LIBRARIES})
		endif()

		# Include directories for install
		target_include_directories(${_target_name} INTERFACE
			$<BUILD_INTERFACE:${${base_target_name}_INCLUDE_DIR}>
			$<BUILD_INTERFACE:${${base_target_name}_DEPS_INCLUDE_DIRS}>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
			$<INSTALL_INTERFACE:include>
			${_shadertoy_subtarget_INTERFACE_INCLUDE_DIRS})

		# Compile definitions
		target_compile_definitions(${_target_name} INTERFACE ${_shadertoy_subtarget_DEFINITIONS})

		# Set library file version
		set_target_properties(${_target_name} PROPERTIES
			SOVERSION ${VERSION_MAJOR}
			VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

		# Set output directory
		set_target_properties(${_target_name} PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY ${SHADERTOY_LIB_DIR}
			LIBRARY_OUTPUT_DIRECTORY ${SHADERTOY_LIB_DIR})

		if (SHADERTOY_STANDALONE AND SHADERTOY_DEFAULT_TARGET_ARG_${_library_type} STREQUAL "")
			# Install target
			install(TARGETS ${_target_name}
					DESTINATION ${CMAKE_INSTALL_LIBDIR}
					EXPORT shadertoy)
		endif()
	endforeach()

	if (SHADERTOY_STANDALONE)
		# Install headers
		install(DIRECTORY ${${base_target_name}_INCLUDE_DIR}/shadertoy
			COMPONENT shadertoy-dev
			DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	endif()

	if (_library_types_count GREATER 1)
		# Rename targets if we use .a archives
		set_target_properties(${base_target_name}-shared PROPERTIES OUTPUT_NAME ${base_target_name}$<$<CONFIG:DEBUG>:d>)
		if(${CMAKE_SHARED_LIBRARY_SUFFIX} MATCHES "\\.so")
			set_target_properties(${base_target_name}-static PROPERTIES OUTPUT_NAME ${base_target_name}$<$<CONFIG:DEBUG>:d>)
		endif()
	endif()
endfunction()

option(SHADERTOY_BUILD_STATIC "Build static libraries" ON)
if (NOT EMSCRIPTEN)
	option(SHADERTOY_BUILD_SHARED "Build shared libraries" ON)
endif()

if (SHADERTOY_STANDALONE)
	set(SHADERTOY_DEFAULT_TARGET_ARG "")
	set(SHADERTOY_DEFAULT_TARGET_ARG_STATIC "")
	set(SHADERTOY_DEFAULT_TARGET_ARG_SHARED "")

	if (NOT SHADERTOY_BUILD_STATIC)
		set(SHADERTOY_DEFAULT_TARGET_ARG_STATIC EXCLUDE_FROM_ALL)
	endif()

	if (NOT SHADERTOY_BUILD_SHARED)
		set(SHADERTOY_DEFAULT_TARGET_ARG_SHARED EXCLUDE_FROM_ALL)
	endif()
else()
	set(SHADERTOY_DEFAULT_TARGET_ARG EXCLUDE_FROM_ALL)
	set(SHADERTOY_DEFAULT_TARGET_ARG_STATIC EXCLUDE_FROM_ALL)
	set(SHADERTOY_DEFAULT_TARGET_ARG_SHARED EXCLUDE_FROM_ALL)
endif()

set(_library_types STATIC)
if (NOT EMSCRIPTEN)
	list(APPEND _library_types SHARED)
endif()
list(LENGTH _library_types _library_types_count)

add_subdirectory(base)
add_subdirectory(backends)
add_subdirectory(core)

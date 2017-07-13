# shadertoy-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# Load oglplus if requested
if (shadertoy_LOAD_OGLPLUS AND NOT shadertoy_OGLPLUS_LOADED)
	set(shadertoy_OGLPLUS_LOADED ON)
	include(${SELF_DIR}/shadertoy-liboglplus.cmake)
endif()

# Include libshadertoy targets
include(${SELF_DIR}/shadertoy.cmake)

# Internal config
get_target_property(shadertoy_INCLUDE_DIR shadertoy INTERFACE_INCLUDE_DIRECTORIES)
set(shadertoy_LIBRARY shadertoy)

include(FindPackageHandleStandardArgs)
# handle QUIETLY and REQUIRED
find_package_handle_standard_args(shadertoy DEFAULT_MSG
	shadertoy_INCLUDE_DIR shadertoy_LIBRARY)

mark_as_advanced(shadertoy_INCLUDE_DIR shadertoy_LIBRARY)
# Set config
set(shadertoy_LIBRARIES ${shadertoy_LIBRARY})
set(shadertoy_INCLUDE_DIRS ${shadertoy_INCLUDE_DIR})

# Try to find oglplus

find_path(oglplus_INCLUDE_DIR oglplus/all.hpp)

include(FindPackageHandleStandardArgs)
# handle QUIETLY and REQUIRED
find_package_handle_standard_args(oglplus DEFAULT_MSG
	oglplus_INCLUDE_DIR)

mark_as_advanced(oglplus_INCLUDE_DIR)
set(oglplus_INCLUDE_DIRS ${oglplus_INCLUDE_DIR})

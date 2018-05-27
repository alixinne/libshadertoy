# Find package for libepoxy
include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(EPOXY_PKGCONF epoxy)

# Include directory
find_path(EPOXY_INCLUDE_DIR
	NAMES epoxy/gl.h
	PATHS ${EPOXY_PKGCONF_INCLUDE_DIRS})

# Finally the library itself
find_library(EPOXY_LIBRARY
	NAMES epoxy
	PATHS ${EPOXY_PKGCONF_LIBRARY_DIRS})

# Set include dir and libraries variables
set(EPOXY_PROCESS_INCLUDES EPOXY_INCLUDE_DIR)
set(EPOXY_PROCESS_LIBS EPOXY_LIBRARY)
libfind_process(EPOXY)


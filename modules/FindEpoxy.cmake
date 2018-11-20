# Find package for libepoxy
include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_detect(EPOXY epoxy
	FIND_PATH epoxy/gl.h
	FIND_LIBRARY epoxy)

# Set include dir and libraries variables
set(Epoxy_PROCESS_INCLUDES EPOXY_INCLUDE_DIR)
set(Epoxy_PROCESS_LIBS EPOXY_LIBRARY)
libfind_process(Epoxy)


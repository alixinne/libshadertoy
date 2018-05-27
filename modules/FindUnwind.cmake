# Find package for libunwind
include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(UNWIND_PKGCONF libunwind)

# Include directory
find_path(UNWIND_INCLUDE_DIR
	NAMES libunwind.h
	PATHS ${UNWIND_PKGCONF_INCLUDE_DIRS})

# Finally unwind library
find_library(UNWIND_LIBRARY
	NAMES unwind
	PATHS ${UNWIND_PKGCONF_LIBRARY_DIRS})

# Find arch-dependent library
include(TargetArch)
target_architecture(UNWIND_ARCH)

find_library(UNWIND_ARCH_LIBRARY
	NAMES unwind-${UNWIND_ARCH}
	PATHS ${UNWIND_PKGCONF_LIBRARY_DIRS})

# Set include dir and libraries variables
set(UNWIND_PROCESS_INCLUDES UNWIND_INCLUDE_DIR)
set(UNWIND_PROCESS_LIBS UNWIND_LIBRARY UNWIND_ARCH_LIBRARY)
set(UNWIND_FIND_QUIETLY ON)
libfind_process(UNWIND)

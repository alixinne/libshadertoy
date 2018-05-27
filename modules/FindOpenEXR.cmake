# Find package for OpenEXR
include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(OpenEXR_PKGCONF OpenEXR)

# Include directory
find_path(OpenEXR_INCLUDE_DIR
	NAMES ImfRgbaFile.h
	PATH_SUFFIXES OpenEXR
	PATHS ${OpenEXR_PKGCONF_INCLUDE_DIRS})

find_library(OpenEXR_HALF_LIBRARY NAMES Half)
find_library(OpenEXR_IEX_LIBRARY NAMES Iex-2_2)
find_library(OpenEXR_IMATH_LIBRARY NAMES Imath-2_2)
find_library(OpenEXR_ILMIMF_LIBRARY NAMES IlmImf-2_2)
find_library(OpenEXR_ILMTHREAD_LIBRARY NAMES IlmThread-2_2)

set(OpenEXR_PROCESS_INCLUDES OpenEXR_INCLUDE_DIR)
set(OpenEXR_PROCESS_LIBS OpenEXR_HALF_LIBRARY OpenEXR_IEX_LIBRARY OpenEXR_IMATH_LIBRARY OpenEXR_ILMIMF_LIBRARY OpenEXR_ILMTHREAD_LIBRARY)
set(OpenEXR_FIND_QUIETLY ON)
libfind_process(OpenEXR)

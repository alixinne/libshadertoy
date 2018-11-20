# Find package for OpenEXR
include(LibFindMacros)

if(WIN32)
	# Use pkg-config to get hints about paths
	libfind_pkg_check_modules(OpenEXR_PKGCONF OpenEXR)

	# Include directory
	find_path(OpenEXR_INCLUDE_DIR
		NAMES ImfRgbaFile.h
		PATH_SUFFIXES OpenEXR
		HINTS ${OpenEXR_PKGCONF_INCLUDE_DIRS})

	# From vcpkg FindOpenEXR.cmake
	find_library(OpenEXR_HALF_LIBRARY NAMES Half
		HINTS ${OpenEXR_PKGCONF_LIBRARY_DIRS})
	find_library(OpenEXR_IEX_LIBRARY NAMES Iex-2_2
		HINTS ${OpenEXR_PKGCONF_LIBRARY_DIRS})
	find_library(OpenEXR_IMATH_LIBRARY NAMES Imath-2_2
		HINTS ${OpenEXR_PKGCONF_LIBRARY_DIRS})
	find_library(OpenEXR_ILMIMF_LIBRARY NAMES IlmImf-2_2
		HINTS ${OpenEXR_PKGCONF_LIBRARY_DIRS})
	find_library(OpenEXR_ILMTHREAD_LIBRARY NAMES IlmThread-2_2
		HINTS ${OpenEXR_PKGCONF_LIBRARY_DIRS})

	set(OpenEXR_PROCESS_LIBS OpenEXR_HALF_LIBRARY OpenEXR_IEX_LIBRARY OpenEXR_IMATH_LIBRARY OpenEXR_ILMIMF_LIBRARY OpenEXR_ILMTHREAD_LIBRARY)
else()
	# Use _detect macro
	libfind_pkg_detect(OpenEXR OpenEXR
		FIND_PATH OpenEXR/ImfRgba.h
		FIND_LIBRARY IlmImf Imath Half Iex IexMath IlmThread)

	set(OpenEXR_PROCESS_LIBS OpenEXR_LIBRARY)
endif()

set(OpenEXR_PROCESS_INCLUDES OpenEXR_INCLUDE_DIR)
libfind_process(OpenEXR)

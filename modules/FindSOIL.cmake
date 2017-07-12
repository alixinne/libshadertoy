# Try to find SOIL, the Simple OpenGL Image Library

find_path(SOIL_INCLUDE_DIR SOIL/SOIL.h)
find_library(SOIL_LIBRARY NAMES SOIL)

include(FindPackageHandleStandardArgs)
# handle QUIETLY and REQUIRED
find_package_handle_standard_args(SOIL DEFAULT_MSG
	SOIL_INCLUDE_DIR SOIL_LIBRARY)

mark_as_advanced(SOIL_INCLUDE_DIR SOIL_LIBRARY)
set(SOIL_LIBRARIES ${SOIL_LIBRARY})
set(SOIL_INCLUDE_DIRS ${SOIL_INCLUDE_DIR})

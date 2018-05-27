# Find package for glm
include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(glm_PKGCONF glm)

# Include directory
find_path(glm_INCLUDE_DIR
	NAMES glm/glm.hpp
	PATHS ${glm_PKGCONF_INCLUDE_DIRS})

# Set include dir variable
set(glm_PROCESS_INCLUDES glm_INCLUDE_DIR)
libfind_process(glm)


# Find GLM using CMake
find_package(glm REQUIRED)

shadertoy_subtarget(stbackend-gx
	INTERFACE_INCLUDE_DIRS "${glm_INCLUDE_DIRS}"
	SUBTARGET_DEPENDENCIES stbase)

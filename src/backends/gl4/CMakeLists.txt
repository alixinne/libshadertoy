# libepoxy extension loader
find_package(Epoxy REQUIRED)

shadertoy_subtarget(stbackend-gl4
	SUBTARGET_DEPENDENCIES stbackend-gx
	PRIVATE_INCLUDE_DIRS "${Epoxy_INCLUDE_DIRS}")

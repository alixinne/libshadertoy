# libOpenImageIO
find_package(OpenImageIO)

# Copy shaders to source
file(GLOB RESOURCES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.cpp
		   ${CMAKE_CURRENT_BINARY_DIR}/resources.hpp
	DEPENDS ${RESOURCES_SOURCES}
	COMMAND ${CMAKE_COMMAND}
		-DRESOURCES_INPUT=${CMAKE_CURRENT_SOURCE_DIR}/shaders
		-DRESOURCES_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/resources.cpp
		-DRESOURCES_H_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/resources.hpp
		-DRESOURCES_H_ID=_SHADERTOY_RESOURCES_HPP_
		-P ${SHADERTOY_CMAKE_DIR}/resources.cmake)

set(_core_INCLUDE_DIRS "")
set(_core_LINK_LIBRARIES "")

if (OpenImageIO_FOUND)
	message(STATUS "Building with OpenImageIO support")
	list(APPEND _core_INCLUDE_DIRS ${OpenImageIO_INCLUDE_DIRS})
	list(APPEND _core_LINK_LIBRARIES ${OIIO_LIBRARIES})
	set(SHADERTOY_HAS_OPENIMAGEIO 1)

	# https://github.com/openexr/openexr/issues/235 still occurs recent installs
	set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/inputs/openimageio_wrapper.cpp
				 PROPERTY COMPILE_FLAGS -std=c++14)
else()
	message(STATUS "Building without OpenImageIO support")
	set(SHADERTOY_HAS_OPENIMAGEIO 0)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/shadertoy/extensions.hpp.in
	${SHADERTOY_BUILT_INCLUDE_DIR}/shadertoy/extensions.hpp @ONLY)

if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	list(APPEND _core_LINK_LIBRARIES stdc++fs)
endif()

shadertoy_subtarget(shadertoy
	INTERFACE_INCLUDE_DIRS "${_core_INCLUDE_DIRS}"
	LINK_LIBRARIES "${_core_LINK_LIBRARIES}"
	EXTRA_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/resources.cpp"
	SUBTARGET_DEPENDENCIES stbackend-gx)

# Enable warnings
if(MSVC)
	target_compile_options(shadertoy-objects PRIVATE /W2 /wd4275 /wd4251)
else()
	target_compile_options(shadertoy-objects PRIVATE -Wall -Wno-missing-braces -Wno-attributes -pedantic)
endif()

if (SHADERTOY_STANDALONE)
	# Install headers
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/shadertoy.hpp
			COMPONENT shadertoy-dev
			DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

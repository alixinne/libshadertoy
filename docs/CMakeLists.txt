find_package(Doxygen)
cmake_dependent_option(SHADERTOY_BUILD_DOCUMENTATION
	"Build the HTML docs" ON
	"DOXYGEN_FOUND" OFF)

if(SHADERTOY_BUILD_DOCUMENTATION)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()

	#set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
	set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/../README.md)
	set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
	set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	configure_file(${doxyfile_in} ${doxyfile} @ONLY)

	add_custom_target(shadertoy_doc
		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)

	if (LIBSHADERTOY_BUILD_DOCUMENTATION)
		install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
				COMPONENT shadertoy-docs
				DESTINATION ${CMAKE_INSTALL_DOCDIR})
	endif()
endif()


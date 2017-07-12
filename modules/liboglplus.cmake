# Load oglplus from a local installation or from a clone of the
# source repository.

find_package(oglplus)
if (NOT oglplus_FOUND)
	# oglplus was not found, get it from the source repositories
	set(oglplus_GIT "https://github.com/matus-chochlik/oglplus.git")
	set(oglplus_TAG "0.69.0")
	set(oglplus_TARGET "${CMAKE_CURRENT_BINARY_DIR}/oglplus")

	message("-- oglplus not found, using local copy from ${oglplus_GIT}")
	
	set(oglplus_INCLUDE_DIRS
		${oglplus_TARGET}/include
		${oglplus_TARGET}/implement
		${CMAKE_CURRENT_BINARY_DIR}/_build/include)

	if (NOT EXISTS ${oglplus_TARGET})
		message("-- fetching oglplus ${oglplus_TAG} from ${oglplus_GIT}")
		execute_process(COMMAND git clone --depth=1
									--branch ${oglplus_TAG}
									${oglplus_GIT}
									${oglplus_TARGET}
						RESULT_VARIABLE oglplus_GIT_CLONE_RESULT)
		if (NOT oglplus_GIT_CLONE_RESULT EQUAL 0)
			message(FATAL_ERROR "Could not fetch oglplus source from ${oglplus_GIT}")
		endif()
	endif()

	if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/_build/include)
		message("-- Configuring oglplus")
		execute_process(COMMAND ${oglplus_TARGET}/configure.py
								--no-examples
								--no-docs
								--use-glew
								--quiet
								--build)
	else()
		message("-- oglplus is already configured. Remove ${CMAKE_CURRENT_SOURCE_DIR}/_build to reconfigure.")
	endif()

	set(oglplus_FOUND TRUE)
endif()

file(GLOB XSCREENSAVER_H ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB XSCREENSAVER_C ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(xscreensaver-support ${XSCREENSAVER_H} ${XSCREENSAVER_C})

target_include_directories(xscreensaver-support PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${X11_INCLUDE_DIRS})

target_compile_definitions(xscreensaver-support PUBLIC
	GETTIMEOFDAY_TWO_ARGS=1
	USE_GL=1
	HAVE_GL=1
	MESA=1)

target_link_libraries(xscreensaver-support PUBLIC
	${X11_LIBRARIES}
	${X11_Xft_LIB}
	${X11_Xt_LIB}
	${X11_Xmu_LIB})

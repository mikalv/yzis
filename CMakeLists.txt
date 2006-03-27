project(yzis)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#non-required
find_package(KDE4)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(${QT_DEFINITIONS})

include(ConvenienceLibs.cmake)

#include(ManualStuff.cmake)
include(ConfigureChecks.cmake)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/yzismacros.h.in ${CMAKE_CURRENT_BINARY_DIR}/yzismacros.h )
macro_additional_clean_files( ${CMAKE_CURRENT_BINARY_DIR}/config.h ${CMAKE_CURRENT_BINARY_DIR}/yzismacros.h )

add_subdirectory( libyzis )

find_package(Ncurses)
if (NOT NCURSESW_FOUND AND NOT HAVE_NCURSESW)
	message (STATUS "The ncurses library does not support wide-character, nyzis will not be compiled.")
else (NOT NCURSESW_FOUND AND NOT HAVE_NCURSESW)
	add_subdirectory( nyzis )
endif (NOT NCURSESW_FOUND AND NOT HAVE_NCURSESW)


#conditional KDE build
if (KDE4_FOUND)
	add_definitions(${KDE4_DEFINITIONS})
	link_directories(${KDE4_LIB_DIR})
	include_directories( ${KDE4_INCLUDE_DIR} )
	add_subdirectory( kyzis )
endif (KDE4_FOUND)

add_subdirectory( translations )
add_subdirectory( qyzis )

include_directories( ${QT_INCLUDES} )

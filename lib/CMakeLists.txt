############################################################################
macro(LinkAllDirectories _DIR)
	LINK_DIRECTORIES("${_DIR}")
	unset(FULL_LIST)
	list(APPEND FULL_LIST "")

	
	file(GLOB SUB_DIR_LIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}" LIST_DIRECTORIES true 
		"${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}*"
	)

	foreach(SUB_DIR ${SUB_DIR_LIST})
		if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}${SUB_DIR})
			list(APPEND FULL_LIST "${_DIR}${SUB_DIR}")
		endif()
	endforeach()

	
	foreach(DIR ${FULL_LIST})
		if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
			LinkAllDirectories("${DIR}")
		endif()
	endforeach()
endmacro()
############################################################################

include_directories (.)

############################################################################				# Dependencies

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

############# Boost
find_package(Boost COMPONENTS filesystem system thread)

if(NOT Boost_FOUND)
	include_directories("../thirdparty/inc")

	if (CMAKE_SIZEOF_VOID_P MATCHES 8) # if x64
		LinkAllDirectories(../thirdparty/lib/x64/)
	else()	# if x86
		LinkAllDirectories(../thirdparty/lib/x86/)
	endif()
	set(LIBS ${LIBS} Boost filesystem system thread)
else()
	include_directories(Boost_INCLUDE_DIRS)
	set(LIBS ${LIBS} ${Boost_LIBRARIES})
endif()

############# Qt
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
set(LIBS ${LIBS} Qt5::Widgets Qt5::Gui Qt5::Core)

############################################################################

#
# The source files
#
file(GLOB_RECURSE  SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" LIST_DIRECTORIES false 
	"*"
)

#
# The header files
#
file(GLOB_RECURSE  HEADER_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" LIST_DIRECTORIES false 
	"../../inc/${PROJECT_FOLDER_NAME}/*"
)

#
# The config
#
configure_file(Configuration.h.in ${PROJECT_SOURCE_DIR}/lib/Configuration.h)

#
# Link
#
add_library(degate SHARED ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(degate ${LIBS})

############################################################################
#
# Documentation
#
#find_package(Doxygen QUIET)
#IF (DOXYGEN_FOUND)
#   add_custom_target(libdoc 
#     COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_SOURCE_DIR}/doc/Doxyfile.libdegate"
#     SOURCES ${SOURCES})
#else()
#   message("Warning: Doxygen not found. Ignoring. You should "
#   	"install the doxygen package to generate the documentation for libdegate.")
#ENDIF(DOXYGEN_FOUND)


if(WIN32)
    set(_library_dir bin)   # .dll are in PATH, like executables
else()
    set(_library_dir lib)
endif()

set(LIB_INSTALL_DIR "${_library_dir}${LIB_SUFFIX}")

set(_INSTALL_DESTINATIONS
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION "lib${LIB_SUFFIX}"
)

install(TARGETS degate ${_INSTALL_DESTINATIONS})


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
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS filesystem system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(LIBS ${Boost_LIBRARIES})

############# Qt
find_package(Qt5 COMPONENTS Core Widgets Gui Xml REQUIRED)
set(LIBS ${LIBS} Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Xml)

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
add_executable(degate ${SRC_FILES} ${HEADER_FILES})
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

